#pragma once

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

class WatchdogService;

class FirmwareUpdateService {
public:
  explicit FirmwareUpdateService(WatchdogService &watchdog);

  void beginUpload(const String &filename);
  void writeChunk(uint8_t *data, size_t size);
  void finishUpload();
  void abortUpload();

  bool uploadStarted() const;
  bool uploadSucceeded() const;
  int responseStatusCode() const;
  const String &message() const;

private:
  void fail(const String &message, int responseStatusCode, bool abortUpdate);

  WatchdogService &watchdog_;
  bool uploadStarted_ = false;
  bool uploadActive_ = false;
  bool uploadSucceeded_ = false;
  int responseStatusCode_ = 400;
  size_t bytesWritten_ = 0;
  String message_;
};


