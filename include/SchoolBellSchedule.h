#pragma once

#include <stdint.h>

#include "CalendarDate.h"
#include "SchoolBellModel.h"
#include "TurkishHolidays.h"

enum class SchoolDayState : uint8_t {
  schoolDay = 0,
  // Excluded by the selected days.
  notSchoolDay = 1,
  vacation = 2,
  publicHoliday = 3,
  // Bells ring until 13:00 only.
  halfDayHoliday = 4
};

enum class SchoolBellKind : uint8_t {
  lesson = 0,
  breakTime = 1,
  studentCall = 2,
  remembrance = 3
};

struct SchoolBellEvent {
  uint16_t minuteOfDay = 0;
  SchoolBellKind kind = SchoolBellKind::lesson;
  // Zero-based lesson index; unused for the remembrance bell.
  uint8_t lesson = 0;
};

// Pure schedule rules shared by the relay logic and the portal status.
// Days are day numbers from CalendarDate.h.
namespace SchoolBellSchedule {

SchoolDayState dayState(
  const SchoolBellSettings &settings,
  int32_t day,
  TurkishHolidayInfo &holiday);

// The bell that rings at this minute, if any.
bool bellAt(
  const SchoolBellSettings &settings,
  int32_t day,
  uint16_t minuteOfDay,
  SchoolBellEvent &event);

// The first bell after the given minute within the next year.
bool nextBell(
  const SchoolBellSettings &settings,
  int32_t day,
  uint16_t afterMinute,
  int32_t &bellDay,
  SchoolBellEvent &event);

} // namespace SchoolBellSchedule
