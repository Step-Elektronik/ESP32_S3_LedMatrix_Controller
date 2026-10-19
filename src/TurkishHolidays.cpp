#include "TurkishHolidays.h"

#include <cstddef>

#include "UiText.h"

namespace {

struct FixedHoliday {
  uint8_t month;
  uint8_t day;
  TurkishHoliday holiday;
  bool halfDay;
};

// Law 2429, articles 1 and 2. Cumhuriyet Bayramı starts on 28 October at 13:00.
constexpr FixedHoliday fixedHolidays[] = {
  {1, 1, TurkishHoliday::newYear, false},
  {4, 23, TurkishHoliday::nationalSovereignty, false},
  {5, 1, TurkishHoliday::labourDay, false},
  {5, 19, TurkishHoliday::youthAndSports, false},
  {7, 15, TurkishHoliday::democracy, false},
  {8, 30, TurkishHoliday::victory, false},
  {10, 28, TurkishHoliday::republicEve, true},
  {10, 29, TurkishHoliday::republic, false}
};

// First days of the feasts from the Diyanet İşleri Başkanlığı "Dini Günler
// Listesi" (vakithesaplama.diyanet.gov.tr) for 2025–2035. The eve before
// each feast is a half-day holiday. 2033 has two Ramazan Bayramı.
constexpr CalendarDate ramadanFeasts[] = {
  {2025, 3, 30}, {2026, 3, 20}, {2027, 3, 9}, {2028, 2, 26},
  {2029, 2, 14}, {2030, 2, 4}, {2031, 1, 24}, {2032, 1, 14},
  {2033, 1, 2}, {2033, 12, 23}, {2034, 12, 12}, {2035, 12, 1}
};
constexpr CalendarDate sacrificeFeasts[] = {
  {2025, 6, 6}, {2026, 5, 27}, {2027, 5, 16}, {2028, 5, 5},
  {2029, 4, 24}, {2030, 4, 13}, {2031, 4, 2}, {2032, 3, 22},
  {2033, 3, 11}, {2034, 3, 1}, {2035, 2, 18}
};
constexpr int32_t ramadanFeastDays = 3;
constexpr int32_t sacrificeFeastDays = 4;

// Returns true when the day is one of the feast days; otherwise marks an
// eve in result unless another holiday was already found.
bool checkFeasts(
  const CalendarDate *feasts,
  size_t count,
  int32_t length,
  TurkishHoliday eve,
  TurkishHoliday feast,
  int32_t day,
  TurkishHolidayInfo &result)
{
  for (size_t index = 0; index < count; ++index) {
    const int32_t first = daysFromCivil(feasts[index]);
    if (day >= first && day < first + length) {
      result.holiday = feast;
      result.halfDay = false;
      return true;
    }
    if (day == first - 1 && result.holiday == TurkishHoliday::none) {
      result.holiday = eve;
      result.halfDay = true;
    }
  }
  return false;
}

} // namespace

namespace TurkishHolidays {

TurkishHolidayInfo lookup(const CalendarDate &date)
{
  TurkishHolidayInfo result;
  for (const FixedHoliday &holiday : fixedHolidays) {
    if (holiday.month == date.month && holiday.day == date.day) {
      result.holiday = holiday.holiday;
      result.halfDay = holiday.halfDay;
      if (!holiday.halfDay) {
        return result;
      }
    }
  }

  // A full feast day outranks a half day such as 28 October.
  const int32_t day = daysFromCivil(date);
  if (checkFeasts(
        ramadanFeasts,
        sizeof(ramadanFeasts) / sizeof(ramadanFeasts[0]),
        ramadanFeastDays,
        TurkishHoliday::ramadanEve,
        TurkishHoliday::ramadan,
        day,
        result)) {
    return result;
  }
  checkFeasts(
    sacrificeFeasts,
    sizeof(sacrificeFeasts) / sizeof(sacrificeFeasts[0]),
    sacrificeFeastDays,
    TurkishHoliday::sacrificeEve,
    TurkishHoliday::sacrifice,
    day,
    result);
  return result;
}

const char *name(TurkishHoliday holiday)
{
  switch (holiday) {
    case TurkishHoliday::newYear: return UI_TEXT("Yılbaşı");
    case TurkishHoliday::nationalSovereignty: return UI_TEXT("Ulusal Egemenlik ve Çocuk Bayramı");
    case TurkishHoliday::labourDay: return UI_TEXT("Emek ve Dayanışma Günü");
    case TurkishHoliday::youthAndSports: return UI_TEXT("Atatürk'ü Anma, Gençlik ve Spor Bayramı");
    case TurkishHoliday::democracy: return UI_TEXT("Demokrasi ve Millî Birlik Günü");
    case TurkishHoliday::victory: return UI_TEXT("Zafer Bayramı");
    case TurkishHoliday::republicEve: return UI_TEXT("Cumhuriyet Bayramı arifesi");
    case TurkishHoliday::republic: return UI_TEXT("Cumhuriyet Bayramı");
    case TurkishHoliday::ramadanEve: return UI_TEXT("Ramazan Bayramı arifesi");
    case TurkishHoliday::ramadan: return UI_TEXT("Ramazan Bayramı");
    case TurkishHoliday::sacrificeEve: return UI_TEXT("Kurban Bayramı arifesi");
    case TurkishHoliday::sacrifice: return UI_TEXT("Kurban Bayramı");
    case TurkishHoliday::none:
    default: return "";
  }
}

} // namespace TurkishHolidays
