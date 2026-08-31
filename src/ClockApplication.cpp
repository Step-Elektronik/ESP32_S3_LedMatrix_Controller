#include "ClockApplication.h"

#include <Arduino.h>

#include "AppConfig.h"

void ClockApplication::begin()
{
  if (!clock_.begin()) {
    return;
  }

  currentDateTime_ = clock_.readDateTime();
  lastRtcPollTime_ = millis();
  ready_ = true;
}

void ClockApplication::update()
{
  const uint32_t now = millis();
  if (ready_ && now - lastRtcPollTime_ >= AppConfig::rtcPollIntervalMs) {
    lastRtcPollTime_ = now;
    currentDateTime_ = clock_.readDateTime();
  }
}

ClockDateTime ClockApplication::dateTime() const
{
  return currentDateTime_;
}

bool ClockApplication::setDateTime(const ClockDateTime &dateTime)
{
  if (!ready_ || !clock_.adjust(dateTime)) {
    return false;
  }

  currentDateTime_ = dateTime;
  lastRtcPollTime_ = millis();
  return true;
}

bool ClockApplication::ready() const
{
  return ready_;
}


