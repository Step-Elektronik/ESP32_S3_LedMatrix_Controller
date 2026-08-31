#include "FirmwareUpdateService.h"

#include <Update.h>

#include "WatchdogService.h"

namespace {

constexpr char requiredFirmwareFilename[] = "firmware.bin";

String updateErrorMessage(const __FlashStringHelper *prefix)
{
  String message(prefix);
  message += F(": ");
  message += Update.errorString();
  return message;
}

} // namespace

FirmwareUpdateService::FirmwareUpdateService(WatchdogService &watchdog)
  : watchdog_(watchdog)
{
}

void FirmwareUpdateService::beginUpload(const String &filename)
{
  uploadStarted_ = true;
  uploadActive_ = false;
  uploadSucceeded_ = false;
  responseStatusCode_ = 400;
  bytesWritten_ = 0;
  message_ = F("Yazılım dosyası alınamadı.");

  if (filename != requiredFirmwareFilename) {
    fail(
      F("Dosya adı firmware.bin olmalıdır."),
      400,
      false);
    return;
  }

  if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
    fail(updateErrorMessage(F("OTA başlatılamadı")), 500, false);
    return;
  }

  uploadActive_ = true;
  message_ = F("Firmware yükleniyor.");
  Serial.println("OTA upload started: firmware.bin");
  watchdog_.feed();
}

void FirmwareUpdateService::writeChunk(uint8_t *data, size_t size)
{
  if (!uploadActive_ || size == 0) {
    return;
  }

  watchdog_.feed();
  const size_t written = Update.write(data, size);
  watchdog_.feed();

  if (written != size) {
    fail(updateErrorMessage(F("Firmware yazılamadı")), 500, true);
    return;
  }

  bytesWritten_ += written;
}

void FirmwareUpdateService::finishUpload()
{
  if (!uploadActive_) {
    return;
  }

  watchdog_.feed();
  if (bytesWritten_ == 0) {
    fail(F("Firmware dosyası boş."), 400, true);
    return;
  }

  if (!Update.end(true)) {
    fail(updateErrorMessage(F("Firmware doğrulanamadı")), 400, false);
    return;
  }

  watchdog_.feed();
  uploadActive_ = false;
  uploadSucceeded_ = true;
  responseStatusCode_ = 200;
  message_ = F("Güncelleme tamamlandı. Cihaz yeniden başlatılıyor.");
  Serial.printf(
    "OTA upload completed: %u bytes\n",
    static_cast<unsigned int>(bytesWritten_));
}

void FirmwareUpdateService::abortUpload()
{
  if (!uploadStarted_ || uploadSucceeded_) {
    return;
  }

  fail(F("Firmware yüklemesi bağlantı kesildiği için iptal edildi."), 400, true);
}

bool FirmwareUpdateService::uploadStarted() const
{
  return uploadStarted_;
}

bool FirmwareUpdateService::uploadSucceeded() const
{
  return uploadSucceeded_;
}

int FirmwareUpdateService::responseStatusCode() const
{
  return responseStatusCode_;
}

const String &FirmwareUpdateService::message() const
{
  return message_;
}

void FirmwareUpdateService::fail(
  const String &message,
  int responseStatusCode,
  bool abortUpdate)
{
  if (abortUpdate && Update.isRunning()) {
    Update.abort();
  }

  uploadActive_ = false;
  uploadSucceeded_ = false;
  responseStatusCode_ = responseStatusCode;
  message_ = message;
  Serial.println(String(F("ERROR: ")) + message_);
  watchdog_.feed();
}


