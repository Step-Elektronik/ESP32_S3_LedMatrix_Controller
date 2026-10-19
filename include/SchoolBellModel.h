#pragma once

#include <stdint.h>

#include "AlarmModel.h"

// The alarm relay runs either the manual alarm list or the school bell
// schedule ("Okul saati"); the other one stays stored but inactive.
enum class AlarmMode : uint8_t {
  manual = 0,
  schoolBell = 1
};

constexpr uint16_t schoolBellSchemaVersion = 1;
constexpr uint8_t maximumSchoolLessons = 20;
constexpr uint8_t maximumSchoolVacations = 10;
constexpr uint8_t maximumStudentBellLeadMinutes = 10;
constexpr uint16_t minutesPerDay = 24 * 60;
// Marks a lesson without a break bell.
constexpr uint16_t noSchoolBreak = 0xFFFF;
// On half-day public holidays work and school stop at 13:00.
constexpr uint16_t halfDayHolidayStartMinute = 13 * 60;
// 10 November 09:05: the national moment of silence for Atatürk.
constexpr uint8_t remembranceMonth = 11;
constexpr uint8_t remembranceDay = 10;
constexpr uint16_t remembranceMinute = 9 * 60 + 5;

// Plain aggregates: default member initializers inside the arrays below
// crash the ESP32 GCC (internal compiler error in gimplify).
struct SchoolLesson {
  // Minutes after midnight; the lesson bell rings here.
  uint16_t startMinute;
  // Break bell (end of the lesson) or noSchoolBreak.
  uint16_t breakMinute;
};

// A period without school, e.g. summer or semester holidays. Both days are
// included; dates are stored as YYYYMMDD.
struct SchoolVacation {
  uint32_t firstDate;
  uint32_t lastDate;
};

struct SchoolBellSettings {
  uint16_t schemaVersion = schoolBellSchemaVersion;
  uint8_t lessonCount = 0;
  uint8_t vacationCount = 0;
  ScheduleDayMode dayMode = ScheduleDayMode::weekdays;
  uint8_t customDayMask = alarmWeekdaysMask;
  // One relay time for every bell of the schedule.
  uint16_t durationSeconds = 5;
  // Extra bell this many minutes before each lesson; 0 = off.
  uint8_t studentBellLeadMinutes = 0;
  bool skipPublicHolidays = true;
  bool remembranceBell = false;
  SchoolLesson lessons[maximumSchoolLessons] = {};
  SchoolVacation vacations[maximumSchoolVacations] = {};
};
