#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <atomic>
#include <cstddef>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <stdint.h>

#include "AppConfig.h"

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
  struct Frame {
    uint32_t receivedAt;
    uint16_t length;
    uint8_t data[AppConfig::Modbus::maximumFrameSize];
  };

  void receiveFrame();
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
  QueueHandle_t frameQueue_ = nullptr;
  // receiveFrame() runs in the UART event task; frame_ is used by the loop.
  Frame pendingFrame_ = {};
  Frame frame_ = {};
  std::atomic<uint32_t> droppedFrames_{0};
  bool directionControlEnabled_ = true;
  bool ready_ = false;
};
