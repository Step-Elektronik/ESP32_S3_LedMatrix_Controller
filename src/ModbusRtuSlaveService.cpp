#include "ModbusRtuSlaveService.h"

#include <cstring>
#include <hal/uart_types.h>

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

// Silent interval that ends a frame, in character times. Modbus requires
// 3.5 characters, and at least 1.75 ms above 19200 baud; 2 ms is used as the
// lower bound to tolerate USB-RS485 adapters that pause inside a frame.
uint8_t frameGapSymbols(const CommunicationSettings &settings)
{
  const uint32_t bitsPerCharacter = 1U
    + settings.dataBits
    + (settings.parity == UartParity::none ? 0U : 1U)
    + 1U;
  const uint32_t characterTimeUs =
    (bitsPerCharacter * 1000000UL + settings.baudRate - 1U) / settings.baudRate;
  const uint32_t minimumGapUs = 2000;
  uint32_t symbols = (minimumGapUs + characterTimeUs - 1U) / characterTimeUs;
  if (symbols < 4) {
    symbols = 4;
  }
  return static_cast<uint8_t>(symbols > 100 ? 100 : symbols);
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

  frameQueue_ = xQueueCreate(AppConfig::Modbus::frameQueueLength, sizeof(Frame));
  if (frameQueue_ == nullptr) {
    Serial.println("ERROR: Modbus frame queue could not be allocated.");
    return;
  }

  // Keep the transceiver in receive mode until the UART owns the DIR pin.
  if (directionControlEnabled_) {
    pinMode(AppConfig::Modbus::directionPin, OUTPUT);
    digitalWrite(AppConfig::Modbus::directionPin, LOW);
  } else {
    pinMode(AppConfig::Modbus::directionPin, INPUT);
  }

  serial_.setRxBufferSize(AppConfig::Modbus::receiveBufferSize);
  serial_.begin(
    communication.baudRate,
    serialConfiguration(communication),
    AppConfig::Modbus::rxPin,
    AppConfig::Modbus::txPin);

  bool hardwareDirection = false;
  if (directionControlEnabled_ && AppConfig::Modbus::hardwareDirectionControl) {
    // In RS485 half-duplex mode the UART raises RTS (wired to DE and /RE)
    // while it transmits and releases it when the last stop bit is sent.
    hardwareDirection =
      serial_.setPins(-1, -1, -1, AppConfig::Modbus::directionPin)
      && serial_.setMode(UART_MODE_RS485_HALF_DUPLEX);
    if (!hardwareDirection) {
      Serial.println("ERROR: UART RS485 mode failed; using software DIR control.");
      pinMode(AppConfig::Modbus::directionPin, OUTPUT);
      digitalWrite(AppConfig::Modbus::directionPin, LOW);
    }
  }
  directionControlEnabled_ = directionControlEnabled_ && !hardwareDirection;

  // The UART detects the end of each frame in hardware, so frames are
  // separated correctly even while the main loop is busy.
  const uint8_t gapSymbols = frameGapSymbols(communication);
  if (!serial_.setRxTimeout(gapSymbols)) {
    Serial.println("ERROR: Modbus RX timeout could not be configured.");
  }
  serial_.onReceive([this]() { receiveFrame(); }, true);

  ready_ = true;
  Serial.printf(
    "Modbus RTU slave %u ready: %lu %u%s1, DIR=%s, frame gap %u chars (RX=%d, TX=%d, DIR=%d).\n",
    communication.slaveAddress,
    static_cast<unsigned long>(communication.baudRate),
    communication.dataBits,
    parityName(communication.parity),
    hardwareDirection ? "UART" : (directionControlEnabled_ ? "GPIO" : "NONE"),
    gapSymbols,
    AppConfig::Modbus::rxPin,
    AppConfig::Modbus::txPin,
    AppConfig::Modbus::directionPin);
}

void ModbusRtuSlaveService::update()
{
  if (!ready_) {
    return;
  }

  const uint32_t dropped = droppedFrames_.exchange(0);
  if (dropped > 0) {
    Serial.printf(
      "WARNING: %lu Modbus frame(s) dropped (queue full or frame too long).\n",
      static_cast<unsigned long>(dropped));
  }

  uint32_t stale = 0;
  while (xQueueReceive(frameQueue_, &frame_, 0) == pdTRUE) {
    if (millis() - frame_.receivedAt > AppConfig::Modbus::maximumResponseDelayMs) {
      ++stale;
      continue;
    }
    processFrame();
  }
  if (stale > 0) {
    Serial.printf(
      "WARNING: %lu late Modbus request(s) dropped instead of answered.\n",
      static_cast<unsigned long>(stale));
  }
}

void ModbusRtuSlaveService::receiveFrame()
{
  // Called from the UART event task after the frame gap has elapsed.
  size_t length = 0;
  bool tooLong = false;
  while (serial_.available() > 0) {
    const int value = serial_.read();
    if (value < 0) {
      break;
    }
    if (length < sizeof(pendingFrame_.data)) {
      pendingFrame_.data[length++] = static_cast<uint8_t>(value);
    } else {
      tooLong = true;
    }
  }
  if (length == 0) {
    return;
  }
  if (tooLong) {
    droppedFrames_.fetch_add(1);
    return;
  }

  pendingFrame_.length = static_cast<uint16_t>(length);
  pendingFrame_.receivedAt = millis();
  if (xQueueSend(frameQueue_, &pendingFrame_, 0) != pdTRUE) {
    droppedFrames_.fetch_add(1);
  }
}

void ModbusRtuSlaveService::processFrame()
{
  if (frame_.length < 4) {
    return;
  }

  const uint16_t receivedCrc = static_cast<uint16_t>(
    frame_.data[frame_.length - 2]
    | (frame_.data[frame_.length - 1] << 8));
  if (calculateCrc(frame_.data, frame_.length - 2) != receivedCrc) {
    return;
  }

  const uint8_t address = frame_.data[0];
  if (address != settings_.communication().slaveAddress && address != 0) {
    return;
  }
  const bool broadcast = address == 0;

  switch (frame_.data[1]) {
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
        sendException(frame_.data[1], illegalFunctionException);
      }
      break;
  }
}

void ModbusRtuSlaveService::processReadHoldingRegisters(bool broadcast)
{
  if (broadcast) {
    return;
  }
  if (frame_.length != 8) {
    sendException(readHoldingRegistersFunction, illegalValueException);
    return;
  }

  const uint16_t startAddress = readBigEndian16(&frame_.data[2]);
  const uint16_t quantity = readBigEndian16(&frame_.data[4]);
  if (quantity == 0 || quantity > AppConfig::Modbus::maximumReadRegisters) {
    sendException(readHoldingRegistersFunction, illegalValueException);
    return;
  }

  uint8_t response[3 + AppConfig::Modbus::maximumReadRegisters * 2] = {};
  response[0] = settings_.communication().slaveAddress;
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
  if (frame_.length != 8) {
    if (!broadcast) {
      sendException(writeSingleRegisterFunction, illegalValueException);
    }
    return;
  }

  const uint16_t address = readBigEndian16(&frame_.data[2]);
  const uint16_t value = readBigEndian16(&frame_.data[4]);
  if (!valueBoxes_.writeHoldingRegisters(address, &value, 1)) {
    if (!broadcast) {
      sendException(writeSingleRegisterFunction, illegalAddressException);
    }
    return;
  }

  if (!broadcast) {
    sendFrame(frame_.data, 6);
  }
}

void ModbusRtuSlaveService::processWriteMultipleRegisters(bool broadcast)
{
  if (frame_.length < 9) {
    if (!broadcast) {
      sendException(writeMultipleRegistersFunction, illegalValueException);
    }
    return;
  }

  const uint16_t startAddress = readBigEndian16(&frame_.data[2]);
  const uint16_t quantity = readBigEndian16(&frame_.data[4]);
  const uint8_t byteCount = frame_.data[6];
  if (quantity == 0
      || quantity > AppConfig::Modbus::maximumReadRegisters
      || byteCount != quantity * 2
      || frame_.length != static_cast<size_t>(9 + byteCount)) {
    if (!broadcast) {
      sendException(writeMultipleRegistersFunction, illegalValueException);
    }
    return;
  }

  uint16_t values[AppConfig::Modbus::maximumReadRegisters] = {};
  for (uint16_t index = 0; index < quantity; ++index) {
    values[index] = readBigEndian16(&frame_.data[7 + index * 2]);
  }
  if (!valueBoxes_.writeHoldingRegisters(startAddress, values, quantity)) {
    if (!broadcast) {
      sendException(writeMultipleRegistersFunction, illegalAddressException);
    }
    return;
  }

  if (!broadcast) {
    uint8_t response[6] = {
      settings_.communication().slaveAddress,
      writeMultipleRegistersFunction,
      frame_.data[2],
      frame_.data[3],
      frame_.data[4],
      frame_.data[5]
    };
    sendFrame(response, sizeof(response));
  }
}

void ModbusRtuSlaveService::sendException(
  uint8_t functionCode,
  uint8_t exceptionCode)
{
  const uint8_t response[3] = {
    settings_.communication().slaveAddress,
    static_cast<uint8_t>(functionCode | 0x80U),
    exceptionCode
  };
  sendFrame(response, sizeof(response));
}

void ModbusRtuSlaveService::sendFrame(
  const uint8_t *payload,
  size_t payloadLength)
{
  uint8_t response[AppConfig::Modbus::maximumFrameSize] = {};
  if (payloadLength + 2 > sizeof(response)) {
    return;
  }

  memcpy(response, payload, payloadLength);
  const uint16_t crc = calculateCrc(response, payloadLength);
  response[payloadLength] = static_cast<uint8_t>(crc);
  response[payloadLength + 1] = static_cast<uint8_t>(crc >> 8);

  if (!directionControlEnabled_) {
    // Hardware or transceiver-controlled direction: queue and return.
    serial_.write(response, payloadLength + 2);
    return;
  }

  digitalWrite(AppConfig::Modbus::directionPin, HIGH);
  delayMicroseconds(50);
  serial_.write(response, payloadLength + 2);
  serial_.flush();
  delayMicroseconds(50);
  digitalWrite(AppConfig::Modbus::directionPin, LOW);
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
