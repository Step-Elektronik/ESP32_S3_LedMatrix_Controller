#pragma once

#include <Preferences.h>
#include <cstddef>
#include <stdint.h>

#include "ContentModel.h"

class AlarmApplication;
class ClockApplication;
class SettingsService;

enum class CountdownState : uint8_t {
  idle = 0,
  running = 1,
  paused = 2,
  finished = 3
};

// Countdown session shared by every "countdown" TextBox. All boxes start
// together and count down their own duration; when a box reaches zero the
// alarm relay runs for that box's alarm time. A running session survives a
// reboot: the DS3231 time at start is stored, so time without power counts.
class CountdownApplication {
public:
  CountdownApplication(
    SettingsService &settings,
    ClockApplication &clock,
    AlarmApplication &alarm);

  void begin();
  void update();

  // Starts or resumes; after the time is up it starts a new session.
  bool start();
  // Pauses a running countdown and silences its alarm.
  void pause();
  // Returns to the full duration and silences the alarm.
  void reset();
  // Call after the design changed.
  void reload();

  CountdownState state() const;
  // True while the relay runs because a countdown reached zero.
  bool alarmActive() const;
  bool hasCountdownContent() const;
  uint32_t elapsedMs() const;
  uint32_t remainingSeconds(const ContentItemSettings &item) const;
  // "H:MM:SS", "MM:SS" or "SS", depending on the configured duration.
  void format(const ContentItemSettings &item, char *buffer, size_t bufferSize) const;
  uint32_t revision() const;

  static bool isCountdown(const ContentItemSettings &item);

private:
  uint32_t longestDurationSeconds() const;
  void markExpiredBoxes(uint32_t elapsed);
  void finish();
  void persist();
  void restore();
  bool rtcEpoch(uint32_t &epoch) const;

  SettingsService &settings_;
  ClockApplication &clock_;
  AlarmApplication &alarm_;
  Preferences preferences_;
  CountdownState state_ = CountdownState::idle;
  uint32_t elapsedBeforeMs_ = 0;
  uint32_t runStartedAt_ = 0;
  bool alarmFired_[maxContentItems] = {};
  uint32_t lastElapsedSecond_ = UINT32_MAX;
  uint32_t revision_ = 0;
  bool storageReady_ = false;
};
