#include "TimeSyncApplication.h"

#include <Arduino.h>
#include <esp_sntp.h>
#include <sys/time.h>
#include <time.h>

#include "AppConfig.h"
#include "ClockApplication.h"
#include "SettingsService.h"
#include "WiFiApplication.h"

std::atomic<bool> TimeSyncApplication::syncPending_{false};

TimeSyncApplication::TimeSyncApplication(
  ClockApplication &clock,
  SettingsService &settings,
  const WiFiApplication &wifi)
  : clock_(clock),
    settings_(settings),
    wifi_(wifi)
{
}

void TimeSyncApplication::begin()
{
  sntp_set_time_sync_notification_cb(&TimeSyncApplication::onTimeSynchronized);
}

void TimeSyncApplication::update()
{
  const bool enabled = settings_.timeSync().ntpEnabled;
  const bool connected = wifi_.stationHasIpAddress();

  // Restart SNTP on every new STA connection so the first request is sent
  // immediately instead of after the client's retry back-off.
  if (enabled && connected && !stationWasConnected_) {
    start();
  } else if (!enabled && running_) {
    stop();
  }
  stationWasConnected_ = connected;

  if (syncPending_.exchange(false)) {
    writeRtcFromSystemTime();
  }
}

void TimeSyncApplication::reload()
{
  stop();
  // A changed server or time zone must reach the RTC with the next answer.
  synchronized_ = false;
  if (settings_.timeSync().ntpEnabled && wifi_.stationHasIpAddress()) {
    start();
  }
}

bool TimeSyncApplication::synchronized() const
{
  return synchronized_;
}

uint32_t TimeSyncApplication::secondsSinceSync() const
{
  return synchronized_ ? (millis() - lastSyncAt_) / 1000U : 0;
}

void TimeSyncApplication::onTimeSynchronized(struct timeval *)
{
  // Runs in the lwIP task; the RTC is written later from the main loop.
  syncPending_.store(true);
}

void TimeSyncApplication::start()
{
  const TimeSyncSettings &settings = settings_.timeSync();
  strlcpy(server_, settings.ntpServer.c_str(), sizeof(server_));
  configTzTime(settings.timeZone.c_str(), server_);
  running_ = true;
  Serial.printf(
    "NTP time sync started: server=%s, TZ=%s\n",
    server_,
    settings.timeZone.c_str());
}

void TimeSyncApplication::stop()
{
  if (!running_) {
    return;
  }
  esp_sntp_stop();
  running_ = false;
  Serial.println("NTP time sync stopped.");
}

void TimeSyncApplication::writeRtcFromSystemTime()
{
  const time_t now = time(nullptr);
  struct tm local = {};
  if (localtime_r(&now, &local) == nullptr
      || local.tm_year + 1900 < 2024
      || local.tm_year + 1900 > 2099) {
    return;
  }

  const uint32_t nowMs = millis();
  if (synchronized_
      && nowMs - lastSyncAt_ < AppConfig::TimeSync::rtcWriteMinimumIntervalMs) {
    return;
  }

  const ClockDateTime dateTime = {
    static_cast<uint16_t>(local.tm_year + 1900),
    static_cast<uint8_t>(local.tm_mon + 1),
    static_cast<uint8_t>(local.tm_mday),
    static_cast<uint8_t>(local.tm_hour),
    static_cast<uint8_t>(local.tm_min),
    static_cast<uint8_t>(local.tm_sec)
  };
  if (!clock_.setDateTime(dateTime)) {
    Serial.println("WARNING: NTP time received but the RTC could not be updated.");
    return;
  }

  synchronized_ = true;
  lastSyncAt_ = nowMs;
  Serial.printf(
    "NTP time synchronized; RTC set to %04u-%02u-%02u %02u:%02u:%02u.\n",
    dateTime.year,
    dateTime.month,
    dateTime.day,
    dateTime.hour,
    dateTime.minute,
    dateTime.second);
}
