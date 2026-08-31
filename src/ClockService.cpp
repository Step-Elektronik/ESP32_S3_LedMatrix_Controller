#include "ClockService.h"

#include <Arduino.h>
#include <Wire.h>

#include "AppConfig.h"

bool ClockService::begin()
{
  Wire.begin(AppConfig::rtcSdaPin, AppConfig::rtcSclPin);
  if (!rtc_.begin(&Wire)) {
    Serial.println("ERROR: DS3231 not found. Check SDA, SCL and power.");
    return false;
  }

  if (rtc_.lostPower()) {
    Serial.println("DS3231 lost power; setting it to firmware build time.");
    rtc_.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  ready_ = true;
  return true;
}

ClockTime ClockService::read()
{
  const DateTime now = rtc_.now();
  return {now.hour(), now.minute(), now.second()};
}

ClockDateTime ClockService::readDateTime()
{
  const DateTime now = rtc_.now();
  return {
    now.year(),
    now.month(),
    now.day(),
    now.hour(),
    now.minute(),
    now.second()
  };
}

bool ClockService::adjust(const ClockDateTime &dateTime)
{
  const DateTime value(
    dateTime.year,
    dateTime.month,
    dateTime.day,
    dateTime.hour,
    dateTime.minute,
    dateTime.second
  );
  if (!ready_ || !value.isValid()) {
    return false;
  }

  rtc_.adjust(value);
  return true;
}


