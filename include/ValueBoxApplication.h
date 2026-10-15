#pragma once

#include <atomic>
#include <cstddef>
#include <esp_timer.h>
#include <stdint.h>

#include "ContentModel.h"

class SettingsService;

class ValueBoxApplication {
public:
  explicit ValueBoxApplication(SettingsService &settings);

  void begin();
  void update();
  void reload();

  // Writes the current value with its decimals and unit suffix as UTF-8.
  void formatValue(uint8_t contentIndex, char *buffer, size_t bufferSize) const;
  // Current value before decimal point shifting; false if not a finite
  // ValueBox value (e.g. a float register holding NaN).
  bool currentNumber(uint8_t contentIndex, double &value) const;
  uint32_t revision() const;

  bool readHoldingRegister(uint16_t address, uint16_t &value) const;
  bool writeHoldingRegisters(
    uint16_t startAddress,
    const uint16_t *values,
    uint16_t count);

private:
  struct RuntimeValue {
    // Value source the runtime value belongs to. A saved design keeps the
    // current value of every ValueBox whose source is unchanged.
    bool active = false;
    ValueSource source = ValueSource::modbus;
    ModbusValueType modbusValueType = ModbusValueType::signedInteger16;
    uint16_t registerAddress = 0;
    int32_t initialIntegerValue = 0;
    uint32_t initialFloatBits = 0;

    int32_t integerValue = 0;
    float floatValue = 0.0F;
  };

  static RuntimeValue initialRuntime(const ContentItemSettings &item);
  static bool sameSource(const RuntimeValue &left, const RuntimeValue &right);
  static void sampleDigitalInput(void *context);

  bool findModbusRegister(
    uint16_t address,
    uint8_t &contentIndex,
    uint8_t &wordIndex) const;
  void applyDigitalInputEdges(uint32_t edges);

  SettingsService &settings_;
  RuntimeValue runtime_[maxContentItems] = {};
  uint32_t revision_ = 0;
  bool ready_ = false;

  esp_timer_handle_t sampleTimer_ = nullptr;
  std::atomic<uint32_t> pendingRisingEdges_{0};
  // Debounce state owned by the sampling timer callback.
  bool samplerLevel_ = false;
  uint32_t samplerStableSamples_ = 0;
  bool samplerArmed_ = false;
};
