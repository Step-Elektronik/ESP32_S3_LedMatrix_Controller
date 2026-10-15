#include "TMP112.h"

namespace {

constexpr uint8_t temperaturePointer = 0x00;
constexpr uint8_t configurationPointer = 0x01;

// Byte 1: OS R1 R0 F1 F0 POL TM SD. R1:R0 are read-only and always 11b.
constexpr uint16_t resolutionBits = 0x6000;
// Byte 2: CR1 CR0 AL EM 0 0 0 0. The low nibble always reads 0.
constexpr uint16_t unusedBits = 0x000F;
constexpr uint16_t extendedModeBit = 0x0010;
constexpr uint8_t conversionRateShift = 6;
constexpr float degreesPerLsb = 0.0625F;

} // namespace

bool TMP112::isSupportedAddress(uint8_t address)
{
  return (address >= 0x48 && address <= 0x4B)
      || (address >= 0x40 && address <= 0x43);
}

bool TMP112::begin(TwoWire &wire, uint8_t address)
{
  wire_ = &wire;
  address_ = address;

  uint16_t configuration = 0;
  return readRegister(configurationPointer, configuration)
      && (configuration & resolutionBits) == resolutionBits
      && (configuration & unusedBits) == 0;
}

bool TMP112::configure(ConversionRate rate, bool extendedMode)
{
  // OS=0, F=00, POL=0, TM=0, SD=0 keeps continuous conversion; R1:R0 are
  // written with their read-only value.
  uint16_t configuration = resolutionBits
    | (static_cast<uint16_t>(rate) << conversionRateShift);
  if (extendedMode) {
    configuration |= extendedModeBit;
  }
  if (!writeRegister(configurationPointer, configuration)) {
    return false;
  }

  uint16_t readBack = 0;
  return readRegister(configurationPointer, readBack)
      && (readBack & (0x00C0 | extendedModeBit))
           == (configuration & (0x00C0 | extendedModeBit));
}

bool TMP112::readTemperature(float &temperatureC)
{
  uint16_t raw = 0;
  if (!readRegister(temperaturePointer, raw)) {
    return false;
  }

  // Bit D0 of byte 2 tells which format the latest conversion used: 12-bit
  // left-justified (EM=0) or 13-bit left-justified (EM=1), twos complement.
  const int16_t value = static_cast<int16_t>(raw);
  const int16_t counts = (raw & 0x0001) != 0
    ? static_cast<int16_t>(value >> 3)
    : static_cast<int16_t>(value >> 4);
  temperatureC = counts * degreesPerLsb;
  return true;
}

bool TMP112::readRegister(uint8_t pointer, uint16_t &value)
{
  if (wire_ == nullptr) {
    return false;
  }

  wire_->beginTransmission(address_);
  wire_->write(pointer);
  if (wire_->endTransmission(false) != 0) {
    return false;
  }
  if (wire_->requestFrom(address_, static_cast<uint8_t>(2)) != 2) {
    while (wire_->available()) {
      wire_->read();
    }
    return false;
  }
  const uint8_t msb = static_cast<uint8_t>(wire_->read());
  const uint8_t lsb = static_cast<uint8_t>(wire_->read());
  value = (static_cast<uint16_t>(msb) << 8) | lsb;
  return true;
}

bool TMP112::writeRegister(uint8_t pointer, uint16_t value)
{
  if (wire_ == nullptr) {
    return false;
  }

  wire_->beginTransmission(address_);
  wire_->write(pointer);
  wire_->write(static_cast<uint8_t>(value >> 8));
  wire_->write(static_cast<uint8_t>(value & 0xFF));
  return wire_->endTransmission() == 0;
}
