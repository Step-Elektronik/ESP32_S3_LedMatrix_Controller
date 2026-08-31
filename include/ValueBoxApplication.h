#pragma once

#include <cstddef>
#include <stdint.h>

#include "ContentModel.h"

class SettingsService;

class ValueBoxApplication {
public:
  explicit ValueBoxApplication(SettingsService &settings);

  void begin();
  void update();
  void reload();

  void formatValue(uint8_t contentIndex, char *buffer, size_t bufferSize) const;
  uint32_t revision() const;

  bool readHoldingRegister(uint16_t address, uint16_t &value) const;
  bool writeHoldingRegisters(
    uint16_t startAddress,
    const uint16_t *values,
    uint16_t count);

private:
  struct RuntimeValue {
    int32_t integerValue = 0;
    float floatValue = 0.0F;
  };

  bool findModbusRegister(
    uint16_t address,
    uint8_t &contentIndex,
    uint8_t &wordIndex) const;
  void handleDigitalInputEdge();

  SettingsService &settings_;
  RuntimeValue runtime_[maxContentItems] = {};
  uint32_t revision_ = 0;
  uint32_t digitalInputChangedAt_ = 0;
  bool digitalInputRawState_ = false;
  bool digitalInputStableState_ = false;
  bool digitalInputArmed_ = false;
  bool ready_ = false;
};


