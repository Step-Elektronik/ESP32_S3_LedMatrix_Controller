#include "HDC2022.h"

#include <math.h>

namespace {

constexpr uint8_t temperatureLowRegister = 0x00;
constexpr uint8_t deviceConfigurationRegister = 0x0E;
constexpr uint8_t measurementConfigurationRegister = 0x0F;
constexpr uint8_t manufacturerIdLowRegister = 0xFC;

constexpr uint16_t texasInstrumentsId = 0x5449;
constexpr uint16_t hdc2022DeviceId = 0x07D0;

// Device configuration: SOFT_RES (bit 7). CC[2:0] = 000 is one-shot mode;
// HEAT_EN, DRDY/INT_EN, INT_POL and INT_MODE stay 0.
constexpr uint8_t softResetBit = 0x80;
constexpr uint8_t oneShotConfiguration = 0x00;
// Measurement configuration: TACC[7:6], HACC[5:4], MEAS_CONF[2:1] = 00
// (humidity + temperature), MEAS_TRIG bit 0.
constexpr uint8_t temperatureResolutionShift = 6;
constexpr uint8_t humidityResolutionShift = 4;
constexpr uint8_t measurementTriggerBit = 0x01;

uint32_t temperatureConversionUs(HDC2022::Resolution resolution)
{
  switch (resolution) {
    case HDC2022::Resolution::bits11: return 336;
    case HDC2022::Resolution::bits9: return 208;
    case HDC2022::Resolution::bits14:
    default: return 594;
  }
}

uint32_t humidityConversionUs(HDC2022::Resolution resolution)
{
  switch (resolution) {
    case HDC2022::Resolution::bits11: return 383;
    case HDC2022::Resolution::bits9: return 254;
    case HDC2022::Resolution::bits14:
    default: return 640;
  }
}

} // namespace

bool HDC2022::begin(TwoWire &wire, uint8_t address)
{
  wire_ = &wire;
  address_ = address;

  uint8_t ids[4] = {};
  if (!readRegisters(manufacturerIdLowRegister, ids, sizeof(ids))) {
    return false;
  }
  const uint16_t manufacturerId = static_cast<uint16_t>(ids[1] << 8) | ids[0];
  const uint16_t deviceId = static_cast<uint16_t>(ids[3] << 8) | ids[2];
  return manufacturerId == texasInstrumentsId && deviceId == hdc2022DeviceId;
}

bool HDC2022::configure(
  Resolution temperatureResolution,
  Resolution humidityResolution)
{
  if (!writeRegister(deviceConfigurationRegister, softResetBit)) {
    return false;
  }
  delay(startupTimeMs);

  temperatureResolution_ = temperatureResolution;
  humidityResolution_ = humidityResolution;
  measurementConfiguration_ = static_cast<uint8_t>(
    (static_cast<uint8_t>(temperatureResolution) << temperatureResolutionShift)
    | (static_cast<uint8_t>(humidityResolution) << humidityResolutionShift));
  if (!writeRegister(deviceConfigurationRegister, oneShotConfiguration)
      || !writeRegister(measurementConfigurationRegister, measurementConfiguration_)) {
    return false;
  }

  uint8_t readBack = 0;
  return readRegisters(measurementConfigurationRegister, &readBack, 1)
      && (readBack & 0xF6) == measurementConfiguration_;
}

bool HDC2022::startMeasurement()
{
  return writeRegister(
    measurementConfigurationRegister,
    measurementConfiguration_ | measurementTriggerBit);
}

bool HDC2022::readMeasurement(float &temperatureC, float &relativeHumidity)
{
  uint8_t data[4] = {};
  if (!readRegisters(temperatureLowRegister, data, sizeof(data))) {
    return false;
  }

  // Data registers are little-endian: low byte first.
  const uint16_t rawTemperature = static_cast<uint16_t>(data[1] << 8) | data[0];
  const uint16_t rawHumidity = static_cast<uint16_t>(data[3] << 8) | data[2];
  // SNAS774A Equation 1 and Equation 3.
  temperatureC = rawTemperature / 65536.0F * 165.0F - 40.0F;
  relativeHumidity = rawHumidity / 65536.0F * 100.0F;
  relativeHumidity = fmaxf(0.0F, fminf(100.0F, relativeHumidity));
  return isfinite(temperatureC) && isfinite(relativeHumidity);
}

uint32_t HDC2022::measurementTimeMs() const
{
  const uint32_t totalUs = temperatureConversionUs(temperatureResolution_)
    + humidityConversionUs(humidityResolution_);
  return (totalUs + 999U) / 1000U + 1U;
}

bool HDC2022::readRegisters(uint8_t firstRegister, uint8_t *data, uint8_t length)
{
  if (wire_ == nullptr) {
    return false;
  }

  wire_->beginTransmission(address_);
  wire_->write(firstRegister);
  if (wire_->endTransmission(false) != 0) {
    return false;
  }
  if (wire_->requestFrom(address_, length) != length) {
    while (wire_->available()) {
      wire_->read();
    }
    return false;
  }
  for (uint8_t index = 0; index < length; ++index) {
    data[index] = static_cast<uint8_t>(wire_->read());
  }
  return true;
}

bool HDC2022::writeRegister(uint8_t reg, uint8_t value)
{
  if (wire_ == nullptr) {
    return false;
  }

  wire_->beginTransmission(address_);
  wire_->write(reg);
  wire_->write(value);
  return wire_->endTransmission() == 0;
}
