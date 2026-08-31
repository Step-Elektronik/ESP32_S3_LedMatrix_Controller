#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

class SHT40 {
public:
  static constexpr uint8_t defaultAddress = 0x44;
  static constexpr uint32_t highPrecisionMeasurementTimeMs = 9;

  bool begin(TwoWire &wire = Wire, uint8_t address = defaultAddress);
  bool isConnected();
  bool softReset();
  bool startHighPrecisionMeasurement();
  bool readMeasurement(float &temperatureC, float &relativeHumidity);

private:
  bool sendCommand(uint8_t command);
  static uint8_t calculateCrc(const uint8_t *data, size_t length);

  TwoWire *wire_ = nullptr;
  uint8_t address_ = defaultAddress;
};


