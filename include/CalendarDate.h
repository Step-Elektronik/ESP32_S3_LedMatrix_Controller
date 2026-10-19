#pragma once

#include <stdint.h>

// Gregorian date helpers based on a day number (days since 1970-01-01), so
// date ranges and weekdays can be computed without a full time library.
struct CalendarDate {
  uint16_t year;
  uint8_t month;
  uint8_t day;
};

inline bool isLeapYear(uint16_t year)
{
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

inline uint8_t daysInMonth(uint16_t year, uint8_t month)
{
  static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month < 1 || month > 12) {
    return 0;
  }
  return month == 2 && isLeapYear(year) ? 29 : days[month - 1];
}

inline bool isValidDate(uint16_t year, uint8_t month, uint8_t day)
{
  return year >= 2000 && year <= 2099 && day >= 1 && day <= daysInMonth(year, month);
}

inline int32_t daysFromCivil(const CalendarDate &date)
{
  const int32_t year = static_cast<int32_t>(date.year) - (date.month <= 2 ? 1 : 0);
  const int32_t era = (year >= 0 ? year : year - 399) / 400;
  const uint32_t yearOfEra = static_cast<uint32_t>(year - era * 400);
  const uint32_t month = date.month;
  const uint32_t dayOfYear =
    (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + date.day - 1;
  const uint32_t dayOfEra =
    yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
  return era * 146097 + static_cast<int32_t>(dayOfEra) - 719468;
}

inline CalendarDate civilFromDays(int32_t days)
{
  days += 719468;
  const int32_t era = (days >= 0 ? days : days - 146096) / 146097;
  const uint32_t dayOfEra = static_cast<uint32_t>(days - era * 146097);
  const uint32_t yearOfEra =
    (dayOfEra - dayOfEra / 1460 + dayOfEra / 36524 - dayOfEra / 146096) / 365;
  const uint32_t dayOfYear =
    dayOfEra - (365 * yearOfEra + yearOfEra / 4 - yearOfEra / 100);
  const uint32_t monthIndex = (5 * dayOfYear + 2) / 153;
  const uint32_t day = dayOfYear - (153 * monthIndex + 2) / 5 + 1;
  const uint32_t month = monthIndex < 10 ? monthIndex + 3 : monthIndex - 9;
  const int32_t year = static_cast<int32_t>(yearOfEra) + era * 400 + (month <= 2 ? 1 : 0);
  return CalendarDate{
    static_cast<uint16_t>(year),
    static_cast<uint8_t>(month),
    static_cast<uint8_t>(day)
  };
}

// 0 = Monday … 6 = Sunday; 1970-01-01 was a Thursday.
inline uint8_t mondayBasedWeekday(int32_t days)
{
  return static_cast<uint8_t>(((days % 7) + 7 + 3) % 7);
}

// YYYYMMDD, which sorts like the date itself.
inline uint32_t dateKey(const CalendarDate &date)
{
  return static_cast<uint32_t>(date.year) * 10000UL
    + static_cast<uint32_t>(date.month) * 100UL
    + date.day;
}
