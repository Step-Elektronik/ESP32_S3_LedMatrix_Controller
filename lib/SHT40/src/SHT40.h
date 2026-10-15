#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

// Sensirion SHT40 humidity and temperature sensor (SHT4x datasheet).
class SHT40 {
public:
  // SHT40-AD1B answers at 0x44, SHT40-BD1B at 0x45.
  static constexpr uint8_t defaultAddress = 0x44;
  static constexpr uint8_t alternateAddress = 0x45;

  // Repeatability modes: commands 0xFD, 0xF6 and 0xE0.
  enum class Precision : uint8_t {
    high = 0,
    medium = 1,
    low = 2
  };

  bool begin(TwoWire &wire = Wire, uint8_t address = defaultAddress);
  bool isConnected();
  bool softReset();
  // Reads the 32-bit serial number; a valid CRC identifies an SHT4x.
  bool readSerialNumber(uint32_t &serialNumber);
  bool startMeasurement(Precision precision);
  bool readMeasurement(float &temperatureC, float &relativeHumidity);

  // Maximum measurement duration from the datasheet, rounded up.
  static uint32_t measurementTimeMs(Precision precision);
  static constexpr uint32_t softResetTimeMs = 1;

private:
  bool sendCommand(uint8_t command);
  bool readWords(uint16_t &first, uint16_t &second);
  static uint8_t calculateCrc(const uint8_t *data, size_t length);

  TwoWire *wire_ = nullptr;
  uint8_t address_ = defaultAddress;
};
