#pragma once

#include <stdint.h>

#include "CalendarDate.h"

// Public holidays of Türkiye (Law 2429). Only Turkish holidays are known.
enum class TurkishHoliday : uint8_t {
  none = 0,
  newYear,
  nationalSovereignty,
  labourDay,
  youthAndSports,
  democracy,
  victory,
  republicEve,
  republic,
  ramadanEve,
  ramadan,
  sacrificeEve,
  sacrifice
};

struct TurkishHolidayInfo {
  TurkishHoliday holiday = TurkishHoliday::none;
  // The eve of a bayram and 28 October are holidays from 13:00 only.
  bool halfDay = false;
};

namespace TurkishHolidays {

// Ramazan and Kurban Bayramı follow the Hijri calendar; their dates are
// built in from the Diyanet İşleri Başkanlığı calendar up to this year.
constexpr uint16_t lastReligiousHolidayYear = 2035;

TurkishHolidayInfo lookup(const CalendarDate &date);
// Turkish name; the portal translates it.
const char *name(TurkishHoliday holiday);

} // namespace TurkishHolidays
