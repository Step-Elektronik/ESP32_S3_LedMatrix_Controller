#include "PanelSleepApplication.h"

#include <Arduino.h>
#include <RTClib.h>

#include "ClockApplication.h"
#include "PanelDisplay.h"
#include "SettingsService.h"

PanelSleepApplication::PanelSleepApplication(
  PanelDisplay &panel,
  ClockApplication &clock,
  SettingsService &settings)
  : panel_(panel),
    clock_(clock),
    settings_(settings)
{
}

void PanelSleepApplication::begin()
{
  applySchedule();
}

void PanelSleepApplication::update()
{
  applySchedule();
}

void PanelSleepApplication::setTemporaryWakeOverride(bool enabled)
{
  if (temporaryWakeOverride_ == enabled) {
    return;
  }

  temporaryWakeOverride_ = enabled;
  if (!enabled) {
    // The temporary operation has finished. Restore the scheduled brightness
    // immediately instead of waiting for the next main-loop iteration.
    const bool sleepNow = shouldSleepNow();
    panel_.setBrightness(
      sleepNow ? 0 : settings_.panel().brightness);
  }
}

bool PanelSleepApplication::sleeping() const
{
  return sleeping_;
}

bool PanelSleepApplication::shouldSleepNow() const
{
  const PanelSleepSettings &sleep = settings_.sleep();
  if (!sleep.enabled || !clock_.ready()) {
    return false;
  }

  const ClockDateTime now = clock_.dateTime();
  const DateTime currentDateTime(
    now.year,
    now.month,
    now.day,
    now.hour,
    now.minute,
    now.second);
  const DateTime currentMidnight(now.year, now.month, now.day, 0, 0, 0);
  const uint32_t nowEpoch = currentDateTime.unixtime();
  const uint32_t midnightEpoch = currentMidnight.unixtime();
  uint32_t latestSleepAt = 0;
  uint32_t latestWakeAt = 0;

  // Sleep starts every day. Waking is allowed only on the selected opening
  // days. Looking back one week also reconstructs the correct output state
  // after a restart during a weekend.
  for (uint8_t daysBack = 0; daysBack <= 7; ++daysBack) {
    const uint32_t dayStart = midnightEpoch - daysBack * 86400UL;

    if (latestSleepAt == 0) {
      const uint32_t candidate =
        dayStart + static_cast<uint32_t>(sleep.startMinuteOfDay) * 60UL;
      if (candidate <= nowEpoch) {
        latestSleepAt = candidate;
      }
    }

    if (latestWakeAt == 0) {
      const DateTime candidateDay(dayStart);
      const uint8_t dayOfWeek = candidateDay.dayOfTheWeek();
      const bool openingAllowed = sleep.dayMode == ScheduleDayMode::everyDay
          || (dayOfWeek >= 1 && dayOfWeek <= 5);
      const uint32_t candidate =
        dayStart + static_cast<uint32_t>(sleep.endMinuteOfDay) * 60UL;
      if (openingAllowed && candidate <= nowEpoch) {
        latestWakeAt = candidate;
      }
    }

    if (latestSleepAt != 0 && latestWakeAt != 0) {
      break;
    }
  }

  return latestSleepAt > latestWakeAt;
}

void PanelSleepApplication::applySchedule()
{
  if (!panel_.ready()) {
    return;
  }

  const bool sleepNow = shouldSleepNow();
  if (sleepNow && !temporaryWakeOverride_) {
    // Keep enforcing zero brightness while the scheduled sleep interval is
    // active, except during an explicitly requested temporary panel test.
    if (panel_.brightness() != 0) {
      panel_.setBrightness(0);
    }
  } else if (!sleepNow && initialized_ && sleeping_
      && !temporaryWakeOverride_) {
    panel_.setBrightness(settings_.panel().brightness);
  }

  if (!initialized_ || sleepNow != sleeping_) {
    Serial.println(sleepNow
      ? "Panel sleep interval started; brightness set to zero."
      : "Panel sleep interval ended; configured brightness restored.");
  }

  sleeping_ = sleepNow;
  initialized_ = true;
}


