#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

// Texas Instruments TMP112 digital temperature sensor (SBOS473L).
// The sensor runs in its default continuous conversion mode; the latest
// conversion result can be read at any time.
class TMP112 {
public:
  // ADD0 -> GND, V+, SDA, SCL selects 0x48-0x4B (SOT563 packages).
  // The X2SON-5 TMP112D address variant uses 0x40-0x43 instead.
  static constexpr uint8_t defaultAddress = 0x48;

  // Configuration register CR1:CR0.
  enum class ConversionRate : uint8_t {
    hz0_25 = 0,
    hz1 = 1,
    hz4 = 2,
    hz8 = 3
  };

  static bool isSupportedAddress(uint8_t address);

  // Checks that the device acknowledges and that the read-only converter
  // resolution bits of the configuration register read 11b.
  bool begin(TwoWire &wire = Wire, uint8_t address = defaultAddress);
  // Continuous conversion, comparator mode, fault queue 1; extended mode
  // (13 bit, up to +150 degC) when requested.
  bool configure(ConversionRate rate, bool extendedMode);
  // Decodes both the 12-bit and the 13-bit (extended) register format.
  bool readTemperature(float &temperatureC);

  // Maximum conversion time (11.25 ms in SBOS473L; older documents list
  // 35 ms), used before the first read after configuration.
  static constexpr uint32_t conversionTimeMs = 35;

private:
  bool readRegister(uint8_t pointer, uint16_t &value);
  bool writeRegister(uint8_t pointer, uint16_t value);

  TwoWire *wire_ = nullptr;
  uint8_t address_ = defaultAddress;
};
