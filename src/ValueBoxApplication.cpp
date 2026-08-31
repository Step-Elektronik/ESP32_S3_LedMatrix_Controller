#include "ValueBoxApplication.h"

#include <Arduino.h>
#include <cstring>
#include <limits.h>

#include "AppConfig.h"
#include "SettingsService.h"

ValueBoxApplication::ValueBoxApplication(SettingsService &settings)
  : settings_(settings)
{
}

void ValueBoxApplication::begin()
{
  pinMode(AppConfig::DigitalInput::pin, INPUT_PULLDOWN);
  digitalInputRawState_ = digitalRead(AppConfig::DigitalInput::pin) == HIGH;
  digitalInputStableState_ = digitalInputRawState_;
  digitalInputArmed_ = false;
  digitalInputChangedAt_ = millis();
  ready_ = true;
  reload();
  Serial.printf(
    "Digital counter input ready on GPIO %d without internal pull; waiting for LOW before rising-edge trigger.\n",
    AppConfig::DigitalInput::pin);
}

void ValueBoxApplication::update()
{
  if (!ready_) {
    return;
  }

  const uint32_t now = millis();
  const bool rawState = digitalRead(AppConfig::DigitalInput::pin) == HIGH;
  if (rawState != digitalInputRawState_) {
    digitalInputRawState_ = rawState;
    digitalInputChangedAt_ = now;
  }

  if (!digitalInputArmed_) {
    if (!rawState
        && now - digitalInputChangedAt_ >= AppConfig::DigitalInput::debounceMs) {
      digitalInputStableState_ = false;
      digitalInputArmed_ = true;
    }
    return;
  }

  if (rawState != digitalInputStableState_
      && now - digitalInputChangedAt_ >= AppConfig::DigitalInput::debounceMs) {
    digitalInputStableState_ = rawState;
    if (digitalInputStableState_) {
      handleDigitalInputEdge();
      digitalInputArmed_ = false;
    }
  }
}

void ValueBoxApplication::reload()
{
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    runtime_[index] = {};
    if (index >= content.count) {
      continue;
    }

    const ContentItemSettings &item = content.items[index];
    if (item.boxKind != ContentBoxKind::valueBox) {
      continue;
    }
    runtime_[index].integerValue = item.initialIntegerValue;
    runtime_[index].floatValue = item.initialFloatValue;
  }
  ++revision_;
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

  if (item.valueSource == ValueSource::modbus
      && item.modbusValueType == ModbusValueType::ieee754Float) {
    snprintf(
      buffer,
      bufferSize,
      "%.1f",
      static_cast<double>(runtime_[contentIndex].floatValue));
  } else {
    snprintf(
      buffer,
      bufferSize,
      "%ld",
      static_cast<long>(runtime_[contentIndex].integerValue));
  }
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

  uint32_t bits = 0;
  std::memcpy(&bits, &runtime_[contentIndex].floatValue, sizeof(bits));
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

    uint32_t bits = 0;
    std::memcpy(&bits, &runtime_[contentIndex].floatValue, sizeof(bits));
    if (wordIndex == 0) {
      bits = (bits & 0x0000ffffUL)
        | (static_cast<uint32_t>(values[offset]) << 16);
    } else {
      bits = (bits & 0xffff0000UL) | values[offset];
    }

    float nextValue = 0.0F;
    std::memcpy(&nextValue, &bits, sizeof(nextValue));
    uint32_t previousBits = 0;
    std::memcpy(
      &previousBits,
      &runtime_[contentIndex].floatValue,
      sizeof(previousBits));
    if (previousBits != bits) {
      runtime_[contentIndex].floatValue = nextValue;
      changed = true;
    }
  }

  if (changed) {
    ++revision_;
  }
  return true;
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

void ValueBoxApplication::handleDigitalInputEdge()
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
    if (item.counterDirection == CounterDirection::up) {
      if (value < INT32_MAX) {
        ++value;
        changed = true;
      }
    } else if (value > INT32_MIN) {
      --value;
      changed = true;
    }
  }

  if (changed) {
    ++revision_;
  }
}


