#include "SchoolBellSchedule.h"

namespace {

// nextBell() looks this far ahead, enough to cross a summer holiday.
constexpr int32_t searchDays = 400;

bool scheduledWeekday(const SchoolBellSettings &settings, int32_t day)
{
  const uint8_t weekday = mondayBasedWeekday(day);
  switch (settings.dayMode) {
    case ScheduleDayMode::everyDay:
      return true;
    case ScheduleDayMode::weekdays:
      return weekday < 5;
    case ScheduleDayMode::customDays:
    default:
      return (settings.customDayMask & (1U << weekday)) != 0;
  }
}

bool inVacation(const SchoolBellSettings &settings, uint32_t key)
{
  for (uint8_t index = 0;
       index < settings.vacationCount && index < maximumSchoolVacations;
       ++index) {
    const SchoolVacation &vacation = settings.vacations[index];
    if (key >= vacation.firstDate && key <= vacation.lastDate) {
      return true;
    }
  }
  return false;
}

// Earliest bell in [fromMinute, beforeMinute) regardless of the day state.
// When bells share a minute, a lesson bell outranks a break bell, which
// outranks a student call.
bool earliestBell(
  const SchoolBellSettings &settings,
  const CalendarDate &date,
  uint16_t fromMinute,
  uint16_t beforeMinute,
  SchoolBellEvent &event)
{
  bool found = false;
  auto consider = [&](uint16_t minute, SchoolBellKind kind, uint8_t lesson) {
    if (minute < fromMinute || minute >= beforeMinute) {
      return;
    }
    if (!found
        || minute < event.minuteOfDay
        || (minute == event.minuteOfDay
            && static_cast<uint8_t>(kind) < static_cast<uint8_t>(event.kind))) {
      event.minuteOfDay = minute;
      event.kind = kind;
      event.lesson = lesson;
      found = true;
    }
  };

  const uint8_t lead = settings.studentBellLeadMinutes;
  for (uint8_t index = 0;
       index < settings.lessonCount && index < maximumSchoolLessons;
       ++index) {
    const SchoolLesson &lesson = settings.lessons[index];
    consider(lesson.startMinute, SchoolBellKind::lesson, index);
    if (lesson.breakMinute != noSchoolBreak) {
      consider(lesson.breakMinute, SchoolBellKind::breakTime, index);
    }
    if (lead > 0 && lesson.startMinute >= lead) {
      consider(lesson.startMinute - lead, SchoolBellKind::studentCall, index);
    }
  }
  if (settings.remembranceBell
      && date.month == remembranceMonth
      && date.day == remembranceDay) {
    consider(remembranceMinute, SchoolBellKind::remembrance, 0);
  }
  return found;
}

// Bells ring before this minute of the day; 0 when the day is silent.
uint16_t ringingUntil(const SchoolBellSettings &settings, int32_t day)
{
  TurkishHolidayInfo holiday;
  switch (SchoolBellSchedule::dayState(settings, day, holiday)) {
    case SchoolDayState::schoolDay:
      return minutesPerDay;
    case SchoolDayState::halfDayHoliday:
      return halfDayHolidayStartMinute;
    default:
      return 0;
  }
}

} // namespace

namespace SchoolBellSchedule {

SchoolDayState dayState(
  const SchoolBellSettings &settings,
  int32_t day,
  TurkishHolidayInfo &holiday)
{
  const CalendarDate date = civilFromDays(day);
  holiday = TurkishHolidays::lookup(date);
  if (!scheduledWeekday(settings, day)) {
    return SchoolDayState::notSchoolDay;
  }
  if (inVacation(settings, dateKey(date))) {
    return SchoolDayState::vacation;
  }
  if (settings.skipPublicHolidays && holiday.holiday != TurkishHoliday::none) {
    return holiday.halfDay
      ? SchoolDayState::halfDayHoliday
      : SchoolDayState::publicHoliday;
  }
  return SchoolDayState::schoolDay;
}

bool bellAt(
  const SchoolBellSettings &settings,
  int32_t day,
  uint16_t minuteOfDay,
  SchoolBellEvent &event)
{
  if (minuteOfDay >= ringingUntil(settings, day)) {
    return false;
  }
  return earliestBell(
    settings,
    civilFromDays(day),
    minuteOfDay,
    minuteOfDay + 1,
    event);
}

bool nextBell(
  const SchoolBellSettings &settings,
  int32_t day,
  uint16_t afterMinute,
  int32_t &bellDay,
  SchoolBellEvent &event)
{
  if (settings.lessonCount == 0 && !settings.remembranceBell) {
    return false;
  }
  for (int32_t offset = 0; offset < searchDays; ++offset) {
    const int32_t candidate = day + offset;
    const uint16_t until = ringingUntil(settings, candidate);
    const uint16_t from = offset == 0 ? afterMinute + 1 : 0;
    if (from >= until) {
      continue;
    }
    if (earliestBell(settings, civilFromDays(candidate), from, until, event)) {
      bellDay = candidate;
      return true;
    }
  }
  return false;
}

} // namespace SchoolBellSchedule
