#pragma once

#include <atomic>
#include <stdint.h>

class ClockApplication;
class SettingsService;
class WiFiApplication;
struct timeval;

// Synchronizes the DS3231 from NTP while the STA interface is connected.
// The RTC keeps local time, so the configured POSIX time zone (including
// daylight saving rules) is applied before the time is written.
class TimeSyncApplication {
public:
  TimeSyncApplication(
    ClockApplication &clock,
    SettingsService &settings,
    const WiFiApplication &wifi);

  void begin();
  void update();
  void reload();

  bool synchronized() const;
  uint32_t secondsSinceSync() const;

private:
  static void onTimeSynchronized(struct timeval *time);

  void start();
  void stop();
  void writeRtcFromSystemTime();

  ClockApplication &clock_;
  SettingsService &settings_;
  const WiFiApplication &wifi_;
  // lwIP keeps a pointer to the server name, so it must outlive SNTP.
  char server_[64] = {};
  bool running_ = false;
  bool stationWasConnected_ = false;
  bool synchronized_ = false;
  uint32_t lastSyncAt_ = 0;

  static std::atomic<bool> syncPending_;
};
