#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

// Texas Instruments HDC2022 humidity and temperature sensor (SNAS774A).
// Measurements are triggered one at a time (one-shot mode).
class HDC2022 {
public:
  // ADDR -> GND or floating selects 0x40, ADDR -> VDD selects 0x41.
  static constexpr uint8_t defaultAddress = 0x40;
  static constexpr uint8_t alternateAddress = 0x41;

  // Accuracy options TACC/HACC in the measurement configuration register.
  enum class Resolution : uint8_t {
    bits14 = 0,
    bits11 = 1,
    bits9 = 2
  };

  // Checks the manufacturer ID (0x5449) and device ID (0x07D0).
  bool begin(TwoWire &wire = Wire, uint8_t address = defaultAddress);
  // Soft reset, then one-shot mode with heater and interrupt pin off.
  bool configure(Resolution temperatureResolution, Resolution humidityResolution);
  // Starts one humidity + temperature conversion.
  bool startMeasurement();
  bool readMeasurement(float &temperatureC, float &relativeHumidity);

  // Conversion time of both channels for the configured resolutions,
  // rounded up with margin (datasheet: 594/336/208 us temperature,
  // 640/383/254 us humidity).
  uint32_t measurementTimeMs() const;
  // The device needs 3.5 ms after power-up or reset before measuring.
  static constexpr uint32_t startupTimeMs = 4;

private:
  bool readRegisters(uint8_t firstRegister, uint8_t *data, uint8_t length);
  bool writeRegister(uint8_t reg, uint8_t value);

  TwoWire *wire_ = nullptr;
  uint8_t address_ = defaultAddress;
  uint8_t measurementConfiguration_ = 0;
  Resolution temperatureResolution_ = Resolution::bits14;
  Resolution humidityResolution_ = Resolution::bits14;
};
