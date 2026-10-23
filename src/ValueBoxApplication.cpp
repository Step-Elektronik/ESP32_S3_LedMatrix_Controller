#include "ValueBoxApplication.h"

#include <Arduino.h>
#include <cmath>
#include <cstring>
#include <driver/gpio.h>
#include <esp_rom_crc.h>
#include <limits.h>
#include <memory>
#include <new>

#include "AppConfig.h"
#include "SettingsService.h"

namespace {

constexpr uint32_t debounceSamples =
  AppConfig::DigitalInput::debounceMs * 1000U
  / AppConfig::DigitalInput::samplePeriodUs;
static_assert(debounceSamples > 0, "Debounce time must cover at least one sample");

uint32_t floatBits(float value)
{
  uint32_t bits = 0;
  std::memcpy(&bits, &value, sizeof(bits));
  return bits;
}

bool usesFloat(const ContentItemSettings &item)
{
  return item.valueSource == ValueSource::modbus
      && item.modbusValueType == ModbusValueType::ieee754Float;
}

} // namespace

ValueBoxApplication::ValueBoxApplication(SettingsService &settings)
  : settings_(settings)
{
}

void ValueBoxApplication::begin()
{
  // The board's external pull-down holds the idle level, so the internal
  // pull resistors stay off.
  pinMode(AppConfig::DigitalInput::pin, INPUT);
  samplerLevel_ = digitalRead(AppConfig::DigitalInput::pin) == HIGH;
  samplerStableSamples_ = 0;
  samplerArmed_ = false;
  pendingRisingEdges_.store(0);

  esp_timer_create_args_t timerArguments = {};
  timerArguments.callback = &ValueBoxApplication::sampleDigitalInput;
  timerArguments.arg = this;
  timerArguments.dispatch_method = ESP_TIMER_TASK;
  timerArguments.name = "counter_input";
  esp_err_t result = esp_timer_create(&timerArguments, &sampleTimer_);
  if (result == ESP_OK) {
    result = esp_timer_start_periodic(
      sampleTimer_,
      AppConfig::DigitalInput::samplePeriodUs);
  }
  if (result != ESP_OK) {
    Serial.printf(
      "ERROR: Digital counter sampling timer could not be started: %s\n",
      esp_err_to_name(result));
  }

  ready_ = true;
  reload();
  if (store_.begin()) {
    restoreStoredValues();
  }
  lastStoreCheck_ = millis();
  Serial.printf(
    "Digital counter input ready on GPIO %d without internal pull; %lu ms debounce, sampled every %lu us.\n",
    AppConfig::DigitalInput::pin,
    static_cast<unsigned long>(AppConfig::DigitalInput::debounceMs),
    static_cast<unsigned long>(AppConfig::DigitalInput::samplePeriodUs));
}

void ValueBoxApplication::update()
{
  if (!ready_) {
    return;
  }

  const uint32_t edges = pendingRisingEdges_.exchange(0);
  if (edges > 0) {
    applyDigitalInputEdges(edges);
  }

  const uint32_t now = millis();
  if (now - lastStoreCheck_ >= AppConfig::ValueStore::checkIntervalMs) {
    lastStoreCheck_ = now;
    storeChangedValues();
  }
}

void ValueBoxApplication::reload()
{
  // Keep the current value of every ValueBox whose source did not change, so
  // moving, recoloring or adding boxes does not reset counters or the last
  // Modbus value. Changing the source or the initial value restarts it.
  std::unique_ptr<RuntimeValue[]> previous(
    new (std::nothrow) RuntimeValue[maxContentItems]);
  bool previousUsed[maxContentItems] = {};
  if (previous) {
    for (uint8_t index = 0; index < maxContentItems; ++index) {
      previous[index] = runtime_[index];
    }
  }

  const ContentSettings &content = settings_.content();
  uint8_t preserved = 0;
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    runtime_[index] = {};
    if (index >= content.count) {
      continue;
    }

    const ContentItemSettings &item = content.items[index];
    if (item.boxKind != ContentBoxKind::valueBox) {
      continue;
    }

    RuntimeValue next = initialRuntime(item);
    if (previous) {
      // Prefer the box at the same position, then any other unused match.
      int16_t match = -1;
      if (!previousUsed[index] && sameSource(previous[index], next)) {
        match = index;
      }
      for (uint8_t candidate = 0; match < 0 && candidate < maxContentItems; ++candidate) {
        if (!previousUsed[candidate] && sameSource(previous[candidate], next)) {
          match = candidate;
        }
      }
      if (match >= 0) {
        previousUsed[match] = true;
        next.integerValue = previous[match].integerValue;
        next.floatValue = previous[match].floatValue;
        ++preserved;
      }
    }
    runtime_[index] = next;
  }
  if (preserved > 0) {
    Serial.printf("ValueBox reload kept %u current value(s).\n", preserved);
  }
  ++revision_;
}

bool ValueBoxApplication::clearStoredValues()
{
  return store_.clear();
}

void ValueBoxApplication::formatValue(
  uint8_t contentIndex,
  char *buffer,
  size_t bufferSize) const
{
  if (buffer == nullptr || bufferSize == 0) {
    return;
  }
  buffer[0] = '\0';

  const ContentSettings &content = settings_.content();
  if (contentIndex >= content.count) {
    return;
  }

  const ContentItemSettings &item = content.items[contentIndex];
  if (item.boxKind != ContentBoxKind::valueBox) {
    return;
  }

  const uint8_t decimals = item.valueDecimals > maxValueDecimals
    ? maxValueDecimals
    : item.valueDecimals;
  if (usesFloat(item)) {
    snprintf(
      buffer,
      bufferSize,
      "%.*f",
      static_cast<int>(decimals),
      static_cast<double>(runtime_[contentIndex].floatValue));
  } else if (decimals == 0) {
    snprintf(
      buffer,
      bufferSize,
      "%ld",
      static_cast<long>(runtime_[contentIndex].integerValue));
  } else {
    // Fixed decimal point, e.g. register value 1234 with 2 decimals -> 12.34.
    static constexpr uint32_t divisors[] = {1U, 10U, 100U, 1000U};
    const int64_t value = runtime_[contentIndex].integerValue;
    const uint64_t magnitude = static_cast<uint64_t>(value < 0 ? -value : value);
    snprintf(
      buffer,
      bufferSize,
      "%s%llu.%0*llu",
      value < 0 ? "-" : "",
      static_cast<unsigned long long>(magnitude / divisors[decimals]),
      static_cast<int>(decimals),
      static_cast<unsigned long long>(magnitude % divisors[decimals]));
  }
  strlcat(buffer, item.text, bufferSize);
}

bool ValueBoxApplication::currentNumber(uint8_t contentIndex, double &value) const
{
  const ContentSettings &content = settings_.content();
  if (contentIndex >= content.count
      || content.items[contentIndex].boxKind != ContentBoxKind::valueBox) {
    return false;
  }
  value = usesFloat(content.items[contentIndex])
    ? static_cast<double>(runtime_[contentIndex].floatValue)
    : static_cast<double>(runtime_[contentIndex].integerValue);
  return std::isfinite(value);
}

uint32_t ValueBoxApplication::revision() const
{
  return revision_;
}

bool ValueBoxApplication::readHoldingRegister(
  uint16_t address,
  uint16_t &value) const
{
  uint8_t contentIndex = 0;
  uint8_t wordIndex = 0;
  if (!findModbusRegister(address, contentIndex, wordIndex)) {
    return false;
  }

  const ContentItemSettings &item = settings_.content().items[contentIndex];
  if (item.modbusValueType == ModbusValueType::signedInteger16) {
    value = static_cast<uint16_t>(
      static_cast<int16_t>(runtime_[contentIndex].integerValue));
    return true;
  }

  const uint32_t bits = floatBits(runtime_[contentIndex].floatValue);
  value = wordIndex == 0
    ? static_cast<uint16_t>(bits >> 16)
    : static_cast<uint16_t>(bits & 0xffffU);
  return true;
}

bool ValueBoxApplication::writeHoldingRegisters(
  uint16_t startAddress,
  const uint16_t *values,
  uint16_t count)
{
  if (values == nullptr || count == 0) {
    return false;
  }

  for (uint16_t offset = 0; offset < count; ++offset) {
    const uint32_t address = static_cast<uint32_t>(startAddress) + offset;
    uint8_t contentIndex = 0;
    uint8_t wordIndex = 0;
    if (address > UINT16_MAX
        || !findModbusRegister(
          static_cast<uint16_t>(address),
          contentIndex,
          wordIndex)) {
      return false;
    }
  }

  bool changed = false;
  for (uint16_t offset = 0; offset < count; ++offset) {
    uint8_t contentIndex = 0;
    uint8_t wordIndex = 0;
    findModbusRegister(
      static_cast<uint16_t>(startAddress + offset),
      contentIndex,
      wordIndex);

    const ContentItemSettings &item = settings_.content().items[contentIndex];
    if (item.modbusValueType == ModbusValueType::signedInteger16) {
      const int32_t nextValue = static_cast<int16_t>(values[offset]);
      if (runtime_[contentIndex].integerValue != nextValue) {
        runtime_[contentIndex].integerValue = nextValue;
        changed = true;
      }
      continue;
    }

    const uint32_t previousBits = floatBits(runtime_[contentIndex].floatValue);
    uint32_t bits = previousBits;
    if (wordIndex == 0) {
      bits = (bits & 0x0000ffffUL)
        | (static_cast<uint32_t>(values[offset]) << 16);
    } else {
      bits = (bits & 0xffff0000UL) | values[offset];
    }

    if (previousBits != bits) {
      std::memcpy(&runtime_[contentIndex].floatValue, &bits, sizeof(bits));
      changed = true;
    }
  }

  if (changed) {
    ++revision_;
  }
  return true;
}

ValueBoxApplication::RuntimeValue ValueBoxApplication::initialRuntime(
  const ContentItemSettings &item)
{
  RuntimeValue runtime;
  runtime.active = true;
  runtime.source = item.valueSource;
  runtime.modbusValueType = item.modbusValueType;
  runtime.registerAddress = item.registerAddress;
  runtime.initialIntegerValue = item.initialIntegerValue;
  runtime.initialFloatBits = floatBits(item.initialFloatValue);
  runtime.integerValue = item.initialIntegerValue;
  runtime.floatValue = item.initialFloatValue;
  return runtime;
}

bool ValueBoxApplication::sameSource(
  const RuntimeValue &left,
  const RuntimeValue &right)
{
  if (!left.active || !right.active || left.source != right.source) {
    return false;
  }
  if (left.source == ValueSource::digitalInput) {
    // The counting direction only affects future edges.
    return left.initialIntegerValue == right.initialIntegerValue;
  }
  if (left.registerAddress != right.registerAddress
      || left.modbusValueType != right.modbusValueType) {
    return false;
  }
  return left.modbusValueType == ModbusValueType::ieee754Float
    ? left.initialFloatBits == right.initialFloatBits
    : left.initialIntegerValue == right.initialIntegerValue;
}

uint32_t ValueBoxApplication::storedIdentity(const RuntimeValue &value)
{
  // The fields sameSource() compares: a stored value comes back only into a
  // box that would also keep it across a design save.
  uint32_t words[4] = {
    static_cast<uint32_t>(value.source),
    0,
    0,
    static_cast<uint32_t>(value.initialIntegerValue)};
  if (value.source == ValueSource::modbus) {
    words[1] = static_cast<uint32_t>(value.modbusValueType);
    words[2] = value.registerAddress;
    if (value.modbusValueType == ModbusValueType::ieee754Float) {
      words[3] = value.initialFloatBits;
    }
  }
  return esp_rom_crc32_le(
    0,
    reinterpret_cast<const uint8_t *>(words),
    sizeof(words));
}

uint32_t ValueBoxApplication::valueBits(const RuntimeValue &value)
{
  return value.source == ValueSource::modbus
      && value.modbusValueType == ModbusValueType::ieee754Float
    ? floatBits(value.floatValue)
    : static_cast<uint32_t>(value.integerValue);
}

void ValueBoxApplication::restoreStoredValues()
{
  uint8_t restored = 0;
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    RuntimeValue &value = runtime_[index];
    const ValueBoxStore::Entry &stored = store_.entry(index);
    if (!value.active
        || !stored.valid
        || stored.identity != storedIdentity(value)) {
      continue;
    }
    if (value.source == ValueSource::modbus
        && value.modbusValueType == ModbusValueType::ieee754Float) {
      std::memcpy(&value.floatValue, &stored.bits, sizeof(stored.bits));
    } else {
      value.integerValue = static_cast<int32_t>(stored.bits);
    }
    ++restored;
  }
  if (restored > 0) {
    ++revision_;
    Serial.printf("ValueBox restored %u value(s) from flash.\n", restored);
  }
}

void ValueBoxApplication::storeChangedValues()
{
  if (!store_.ready()) {
    return;
  }
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    const RuntimeValue &value = runtime_[index];
    if (!value.active) {
      if (store_.entry(index).valid) {
        store_.forget(index);
      }
      continue;
    }
    const uint32_t identity = storedIdentity(value);
    const uint32_t bits = valueBits(value);
    const ValueBoxStore::Entry &stored = store_.entry(index);
    if (stored.valid && stored.identity == identity && stored.bits == bits) {
      continue;
    }
    if (!store_.save(index, identity, bits)) {
      return;
    }
  }
}

void ValueBoxApplication::sampleDigitalInput(void *context)
{
  // Runs every samplePeriodUs in the esp_timer task, independent of the main
  // loop. A rising edge counts after the input stayed LOW and then HIGH for
  // the full debounce time; the loop applies the counted edges later.
  auto *self = static_cast<ValueBoxApplication *>(context);
  const bool level =
    gpio_get_level(static_cast<gpio_num_t>(AppConfig::DigitalInput::pin)) != 0;
  if (level != self->samplerLevel_) {
    self->samplerLevel_ = level;
    self->samplerStableSamples_ = 0;
    return;
  }
  if (self->samplerStableSamples_ >= debounceSamples) {
    return;
  }
  if (++self->samplerStableSamples_ < debounceSamples) {
    return;
  }

  if (!level) {
    self->samplerArmed_ = true;
  } else if (self->samplerArmed_) {
    self->samplerArmed_ = false;
    self->pendingRisingEdges_.fetch_add(1);
  }
}

bool ValueBoxApplication::findModbusRegister(
  uint16_t address,
  uint8_t &contentIndex,
  uint8_t &wordIndex) const
{
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    if (item.boxKind != ContentBoxKind::valueBox
        || item.valueSource != ValueSource::modbus) {
      continue;
    }

    if (address == item.registerAddress) {
      contentIndex = index;
      wordIndex = 0;
      return true;
    }
    if (item.modbusValueType == ModbusValueType::ieee754Float
        && item.registerAddress < UINT16_MAX
        && address == static_cast<uint16_t>(item.registerAddress + 1U)) {
      contentIndex = index;
      wordIndex = 1;
      return true;
    }
  }
  return false;
}

void ValueBoxApplication::applyDigitalInputEdges(uint32_t edges)
{
  bool changed = false;
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    if (item.boxKind != ContentBoxKind::valueBox
        || item.valueSource != ValueSource::digitalInput) {
      continue;
    }

    int32_t &value = runtime_[index].integerValue;
    int64_t next = value;
    if (item.counterDirection == CounterDirection::up) {
      next += edges;
      if (next > INT32_MAX) {
        next = INT32_MAX;
      }
    } else {
      next -= edges;
      if (next < INT32_MIN) {
        next = INT32_MIN;
      }
    }
    if (next != value) {
      value = static_cast<int32_t>(next);
      changed = true;
    }
  }

  if (changed) {
    ++revision_;
  }
}
