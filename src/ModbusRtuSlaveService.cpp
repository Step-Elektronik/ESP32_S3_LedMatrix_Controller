#include "ModbusRtuSlaveService.h"

#include <cstring>

#include "AppConfig.h"
#include "SettingsService.h"
#include "ValueBoxApplication.h"

namespace {

constexpr uint8_t readHoldingRegistersFunction = 0x03;
constexpr uint8_t writeSingleRegisterFunction = 0x06;
constexpr uint8_t writeMultipleRegistersFunction = 0x10;
constexpr uint8_t illegalFunctionException = 0x01;
constexpr uint8_t illegalAddressException = 0x02;
constexpr uint8_t illegalValueException = 0x03;

uint16_t readBigEndian16(const uint8_t *data)
{
  return static_cast<uint16_t>(data[0] << 8) | data[1];
}

uint32_t serialConfiguration(const CommunicationSettings &settings)
{
  switch (settings.dataBits) {
    case 5:
      if (settings.parity == UartParity::even) return SERIAL_5E1;
      if (settings.parity == UartParity::odd) return SERIAL_5O1;
      return SERIAL_5N1;
    case 6:
      if (settings.parity == UartParity::even) return SERIAL_6E1;
      if (settings.parity == UartParity::odd) return SERIAL_6O1;
      return SERIAL_6N1;
    case 7:
      if (settings.parity == UartParity::even) return SERIAL_7E1;
      if (settings.parity == UartParity::odd) return SERIAL_7O1;
      return SERIAL_7N1;
    case 8:
    default:
      if (settings.parity == UartParity::even) return SERIAL_8E1;
      if (settings.parity == UartParity::odd) return SERIAL_8O1;
      return SERIAL_8N1;
  }
}

uint32_t calculateFrameGapMs(const CommunicationSettings &settings)
{
  const uint32_t bitsPerCharacter = 1U
    + settings.dataBits
    + (settings.parity == UartParity::none ? 0U : 1U)
    + 1U;
  const uint32_t calculated =
    (bitsPerCharacter * 3500U + settings.baudRate - 1U)
    / settings.baudRate;
  return calculated < 2U ? 2U : calculated;
}

const char *parityName(UartParity parity)
{
  switch (parity) {
    case UartParity::even: return "E";
    case UartParity::odd: return "O";
    case UartParity::none:
    default: return "N";
  }
}

} // namespace

ModbusRtuSlaveService::ModbusRtuSlaveService(
  ValueBoxApplication &valueBoxes,
  SettingsService &settings)
  : valueBoxes_(valueBoxes),
    settings_(settings),
    serial_(1)
{
}

void ModbusRtuSlaveService::begin()
{
  const CommunicationSettings &communication = settings_.communication();
  directionControlEnabled_ =
    communication.handshake == UartHandshake::rs485Direction;
  if (directionControlEnabled_) {
    pinMode(AppConfig::Modbus::directionPin, OUTPUT);
    digitalWrite(AppConfig::Modbus::directionPin, LOW);
  } else {
    pinMode(AppConfig::Modbus::directionPin, INPUT);
  }
  frameGapMs_ = calculateFrameGapMs(communication);
  serial_.begin(
    communication.baudRate,
    serialConfiguration(communication),
    AppConfig::Modbus::rxPin,
    AppConfig::Modbus::txPin);
  ready_ = true;
  Serial.printf(
    "Modbus RTU slave %u ready: %lu %u%s1, handshake=%s (RX=%d, TX=%d, DIR=%d).\n",
    AppConfig::Modbus::slaveAddress,
    static_cast<unsigned long>(communication.baudRate),
    communication.dataBits,
    parityName(communication.parity),
    directionControlEnabled_ ? "RS485_DIR" : "NONE",
    AppConfig::Modbus::rxPin,
    AppConfig::Modbus::txPin,
    AppConfig::Modbus::directionPin);
}

void ModbusRtuSlaveService::update()
{
  if (!ready_) {
    return;
  }

  while (serial_.available() > 0) {
    const int value = serial_.read();
    if (value < 0) {
      break;
    }
    if (receiveLength_ >= receiveBufferSize) {
      receiveLength_ = 0;
    }
    receiveBuffer_[receiveLength_++] = static_cast<uint8_t>(value);
    lastByteAt_ = millis();
  }

  if (receiveLength_ > 0
      && millis() - lastByteAt_ >= frameGapMs_) {
    processFrame();
    receiveLength_ = 0;
  }
}

void ModbusRtuSlaveService::processFrame()
{
  if (receiveLength_ < 4) {
    return;
  }

  const uint16_t receivedCrc = static_cast<uint16_t>(
    receiveBuffer_[receiveLength_ - 2]
    | (receiveBuffer_[receiveLength_ - 1] << 8));
  if (calculateCrc(receiveBuffer_, receiveLength_ - 2) != receivedCrc) {
    return;
  }

  const uint8_t address = receiveBuffer_[0];
  if (address != AppConfig::Modbus::slaveAddress && address != 0) {
    return;
  }
  const bool broadcast = address == 0;

  switch (receiveBuffer_[1]) {
    case readHoldingRegistersFunction:
      processReadHoldingRegisters(broadcast);
      break;
    case writeSingleRegisterFunction:
      processWriteSingleRegister(broadcast);
      break;
    case writeMultipleRegistersFunction:
      processWriteMultipleRegisters(broadcast);
      break;
    default:
      if (!broadcast) {
        sendException(receiveBuffer_[1], illegalFunctionException);
      }
      break;
  }
}

void ModbusRtuSlaveService::processReadHoldingRegisters(bool broadcast)
{
  if (broadcast) {
    return;
  }
  if (receiveLength_ != 8) {
    sendException(readHoldingRegistersFunction, illegalValueException);
    return;
  }

  const uint16_t startAddress = readBigEndian16(&receiveBuffer_[2]);
  const uint16_t quantity = readBigEndian16(&receiveBuffer_[4]);
  if (quantity == 0 || quantity > AppConfig::Modbus::maximumReadRegisters) {
    sendException(readHoldingRegistersFunction, illegalValueException);
    return;
  }

  uint8_t response[3 + AppConfig::Modbus::maximumReadRegisters * 2] = {};
  response[0] = AppConfig::Modbus::slaveAddress;
  response[1] = readHoldingRegistersFunction;
  response[2] = static_cast<uint8_t>(quantity * 2);
  for (uint16_t index = 0; index < quantity; ++index) {
    const uint32_t address = static_cast<uint32_t>(startAddress) + index;
    uint16_t value = 0;
    if (address > UINT16_MAX
        || !valueBoxes_.readHoldingRegister(
          static_cast<uint16_t>(address),
          value)) {
      sendException(readHoldingRegistersFunction, illegalAddressException);
      return;
    }
    response[3 + index * 2] = static_cast<uint8_t>(value >> 8);
    response[4 + index * 2] = static_cast<uint8_t>(value);
  }
  sendFrame(response, 3 + quantity * 2);
}

void ModbusRtuSlaveService::processWriteSingleRegister(bool broadcast)
{
  if (receiveLength_ != 8) {
    if (!broadcast) {
      sendException(writeSingleRegisterFunction, illegalValueException);
    }
    return;
  }

  const uint16_t address = readBigEndian16(&receiveBuffer_[2]);
  const uint16_t value = readBigEndian16(&receiveBuffer_[4]);
  if (!valueBoxes_.writeHoldingRegisters(address, &value, 1)) {
    if (!broadcast) {
      sendException(writeSingleRegisterFunction, illegalAddressException);
    }
    return;
  }

  if (!broadcast) {
    sendFrame(receiveBuffer_, 6);
  }
}

void ModbusRtuSlaveService::processWriteMultipleRegisters(bool broadcast)
{
  if (receiveLength_ < 9) {
    if (!broadcast) {
      sendException(writeMultipleRegistersFunction, illegalValueException);
    }
    return;
  }

  const uint16_t startAddress = readBigEndian16(&receiveBuffer_[2]);
  const uint16_t quantity = readBigEndian16(&receiveBuffer_[4]);
  const uint8_t byteCount = receiveBuffer_[6];
  if (quantity == 0
      || quantity > AppConfig::Modbus::maximumReadRegisters
      || byteCount != quantity * 2
      || receiveLength_ != static_cast<size_t>(9 + byteCount)) {
    if (!broadcast) {
      sendException(writeMultipleRegistersFunction, illegalValueException);
    }
    return;
  }

  uint16_t values[AppConfig::Modbus::maximumReadRegisters] = {};
  for (uint16_t index = 0; index < quantity; ++index) {
    values[index] = readBigEndian16(&receiveBuffer_[7 + index * 2]);
  }
  if (!valueBoxes_.writeHoldingRegisters(startAddress, values, quantity)) {
    if (!broadcast) {
      sendException(writeMultipleRegistersFunction, illegalAddressException);
    }
    return;
  }

  if (!broadcast) {
    uint8_t response[6] = {
      AppConfig::Modbus::slaveAddress,
      writeMultipleRegistersFunction,
      receiveBuffer_[2],
      receiveBuffer_[3],
      receiveBuffer_[4],
      receiveBuffer_[5]
    };
    sendFrame(response, sizeof(response));
  }
}

void ModbusRtuSlaveService::sendException(
  uint8_t functionCode,
  uint8_t exceptionCode)
{
  const uint8_t response[3] = {
    AppConfig::Modbus::slaveAddress,
    static_cast<uint8_t>(functionCode | 0x80U),
    exceptionCode
  };
  sendFrame(response, sizeof(response));
}

void ModbusRtuSlaveService::sendFrame(
  const uint8_t *payload,
  size_t payloadLength)
{
  uint8_t response[receiveBufferSize] = {};
  if (payloadLength + 2 > sizeof(response)) {
    return;
  }

  memcpy(response, payload, payloadLength);
  const uint16_t crc = calculateCrc(response, payloadLength);
  response[payloadLength] = static_cast<uint8_t>(crc);
  response[payloadLength + 1] = static_cast<uint8_t>(crc >> 8);

  if (directionControlEnabled_) {
    digitalWrite(AppConfig::Modbus::directionPin, HIGH);
    delayMicroseconds(50);
  }
  serial_.write(response, payloadLength + 2);
  serial_.flush();
  if (directionControlEnabled_) {
    delayMicroseconds(50);
    digitalWrite(AppConfig::Modbus::directionPin, LOW);
  }
}

uint16_t ModbusRtuSlaveService::calculateCrc(
  const uint8_t *data,
  size_t length)
{
  uint16_t crc = 0xffffU;
  for (size_t index = 0; index < length; ++index) {
    crc ^= data[index];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      crc = (crc & 1U) != 0
        ? static_cast<uint16_t>((crc >> 1) ^ 0xa001U)
        : static_cast<uint16_t>(crc >> 1);
    }
  }
  return crc;
}


