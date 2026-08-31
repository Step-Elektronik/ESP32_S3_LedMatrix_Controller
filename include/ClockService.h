#pragma once

#include <stdint.h>
#include <RTClib.h>

struct ClockTime {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

struct ClockDateTime {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

class ClockService {
public:
  bool begin();
  ClockTime read();
  ClockDateTime readDateTime();
  bool adjust(const ClockDateTime &dateTime);

private:
  RTC_DS3231 rtc_;
  bool ready_ = false;
};


