#pragma once

#include <stdint.h>

#include "AlarmModel.h"

class ClockApplication;
class SettingsService;

class AlarmApplication {
public:
  AlarmApplication(ClockApplication &clock, SettingsService &settings);

  void begin();
  void update();
  bool relayActive() const;

private:
  static uint32_t dateKey(const struct ClockDateTime &dateTime);
  static uint32_t configurationSignature(const AlarmItemSettings &alarm);
  static bool isWeekday(const struct ClockDateTime &dateTime);
  static bool isScheduledForDay(
    const AlarmItemSettings &alarm,
    const struct ClockDateTime &dateTime);

  void trigger(uint8_t alarmIndex, const AlarmItemSettings &alarm);
  void setRelay(bool active);

  ClockApplication &clock_;
  SettingsService &settings_;
  uint32_t lastTriggeredDate_[maximumAlarmCount] = {};
  uint32_t configurationSignatures_[maximumAlarmCount] = {};
  bool configurationInitialized_[maximumAlarmCount] = {};
  uint32_t relayOffAt_ = 0;
  bool relayActive_ = false;
  bool ready_ = false;
};


