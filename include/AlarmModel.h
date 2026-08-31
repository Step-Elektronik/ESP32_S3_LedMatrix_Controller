#pragma once

#include <stdint.h>

#include "ScheduleModel.h"

constexpr uint16_t alarmSchemaVersion = 2;
constexpr uint8_t maximumAlarmCount = 10;
constexpr uint16_t minimumAlarmDurationSeconds = 1;
constexpr uint16_t maximumAlarmDurationSeconds = 3600;
constexpr uint8_t alarmWeekdaysMask = 0x1F;
constexpr uint8_t alarmAllDaysMask = 0x7F;

struct AlarmItemSettings {
  uint8_t hour = 8;
  uint8_t minute = 0;
  uint16_t durationSeconds = 5;
  ScheduleDayMode dayMode = ScheduleDayMode::everyDay;
  uint8_t customDayMask = alarmAllDaysMask;
};

struct AlarmSettings {
  uint16_t schemaVersion = alarmSchemaVersion;
  uint8_t count = 0;
  AlarmItemSettings items[maximumAlarmCount] = {};
};


