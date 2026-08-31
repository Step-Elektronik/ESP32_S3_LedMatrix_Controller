#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <cstddef>
#include <stdint.h>

class ValueBoxApplication;
class SettingsService;

class ModbusRtuSlaveService {
public:
  ModbusRtuSlaveService(
    ValueBoxApplication &valueBoxes,
    SettingsService &settings);

  void begin();
  void update();

private:
  static constexpr size_t receiveBufferSize = 256;

  void processFrame();
  void processReadHoldingRegisters(bool broadcast);
  void processWriteSingleRegister(bool broadcast);
  void processWriteMultipleRegisters(bool broadcast);
  void sendException(uint8_t functionCode, uint8_t exceptionCode);
  void sendFrame(const uint8_t *payload, size_t payloadLength);
  static uint16_t calculateCrc(const uint8_t *data, size_t length);

  ValueBoxApplication &valueBoxes_;
  SettingsService &settings_;
  HardwareSerial serial_;
  uint8_t receiveBuffer_[receiveBufferSize] = {};
  size_t receiveLength_ = 0;
  uint32_t lastByteAt_ = 0;
  uint32_t frameGapMs_ = 5;
  bool directionControlEnabled_ = true;
  bool ready_ = false;
};


