#include "SHT40.h"

#include <math.h>

namespace {

constexpr uint8_t softResetCommand = 0x94;
constexpr uint8_t highPrecisionMeasurementCommand = 0xFD;
constexpr uint8_t responseLength = 6;
constexpr uint8_t crcPolynomial = 0x31;

} // namespace

bool SHT40::begin(TwoWire &wire, uint8_t address)
{
  wire_ = &wire;
  address_ = address;
  return isConnected();
}

bool SHT40::isConnected()
{
  if (wire_ == nullptr) {
    return false;
  }

  wire_->beginTransmission(address_);
  return wire_->endTransmission() == 0;
}

bool SHT40::softReset()
{
  return sendCommand(softResetCommand);
}

bool SHT40::startHighPrecisionMeasurement()
{
  return sendCommand(highPrecisionMeasurementCommand);
}

bool SHT40::readMeasurement(
  float &temperatureC,
  float &relativeHumidity)
{
  if (wire_ == nullptr) {
    return false;
  }

  const uint8_t received = wire_->requestFrom(address_, responseLength);
  if (received != responseLength) {
    while (wire_->available()) {
      wire_->read();
    }
    return false;
  }

  uint8_t data[responseLength] = {};
  for (uint8_t index = 0; index < responseLength; ++index) {
    data[index] = static_cast<uint8_t>(wire_->read());
  }
  if (calculateCrc(data, 2) != data[2]
      || calculateCrc(data + 3, 2) != data[5]) {
    return false;
  }

  const uint16_t rawTemperature =
    (static_cast<uint16_t>(data[0]) << 8) | data[1];
  const uint16_t rawHumidity =
    (static_cast<uint16_t>(data[3]) << 8) | data[4];

  temperatureC = -45.0F + 175.0F * rawTemperature / 65535.0F;
  relativeHumidity = -6.0F + 125.0F * rawHumidity / 65535.0F;
  relativeHumidity = fmaxf(0.0F, fminf(100.0F, relativeHumidity));
  return isfinite(temperatureC) && isfinite(relativeHumidity);
}

bool SHT40::sendCommand(uint8_t command)
{
  if (wire_ == nullptr) {
    return false;
  }

  wire_->beginTransmission(address_);
  wire_->write(command);
  return wire_->endTransmission() == 0;
}

uint8_t SHT40::calculateCrc(const uint8_t *data, size_t length)
{
  uint8_t crc = 0xFF;
  for (size_t index = 0; index < length; ++index) {
    crc ^= data[index];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      crc = (crc & 0x80) != 0
        ? static_cast<uint8_t>((crc << 1) ^ crcPolynomial)
        : static_cast<uint8_t>(crc << 1);
    }
  }
  return crc;
}


