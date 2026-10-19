#pragma once

#include <stdint.h>

#include "AlarmModel.h"
#include "SchoolBellModel.h"

class ClockApplication;
class SettingsService;

class AlarmApplication {
public:
  AlarmApplication(ClockApplication &clock, SettingsService &settings);

  void begin();
  void update();
  bool relayActive() const;

  // The countdown shares the relay with the scheduled alarms; the relay
  // stays on while either of them needs it.
  void activateCountdownAlarm(uint16_t durationSeconds);
  void stopCountdownAlarm();
  bool countdownAlarmActive() const;
  // Runs the relay now, e.g. to test the bell from the portal.
  void testRelay(uint16_t durationSeconds);

private:
  static uint32_t dateKey(const struct ClockDateTime &dateTime);
  static uint32_t configurationSignature(const AlarmItemSettings &alarm);
  static bool isWeekday(const struct ClockDateTime &dateTime);
  static bool isScheduledForDay(
    const AlarmItemSettings &alarm,
    const struct ClockDateTime &dateTime);

  void updateManualAlarms(const struct ClockDateTime &now);
  void updateSchoolBell(const struct ClockDateTime &now);
  void trigger(uint8_t alarmIndex, const AlarmItemSettings &alarm);
  void activateScheduledRelay(uint16_t durationSeconds);
  void updateRelayOutput();

  ClockApplication &clock_;
  SettingsService &settings_;
  uint32_t lastTriggeredDate_[maximumAlarmCount] = {};
  uint32_t configurationSignatures_[maximumAlarmCount] = {};
  bool configurationInitialized_[maximumAlarmCount] = {};
  // The school bell checks every minute once and never rings twice for
  // the same minute, even if the clock is set back.
  int32_t lastSchoolBellDay_ = INT32_MIN;
  uint16_t lastSchoolBellMinute_ = UINT16_MAX;
  int32_t lastRungSchoolBellDay_ = INT32_MIN;
  uint16_t lastRungSchoolBellMinute_ = UINT16_MAX;
  uint32_t scheduledRelayOffAt_ = 0;
  bool scheduledRelayActive_ = false;
  uint32_t countdownRelayOffAt_ = 0;
  bool countdownRelayActive_ = false;
  bool relayOutputActive_ = false;
  bool ready_ = false;
};
