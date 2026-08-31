#include "AlarmApplication.h"

#include <Arduino.h>
#include <RTClib.h>

#include "AppConfig.h"
#include "ClockApplication.h"
#include "SettingsService.h"

AlarmApplication::AlarmApplication(
  ClockApplication &clock,
  SettingsService &settings)
  : clock_(clock),
    settings_(settings)
{
}

void AlarmApplication::begin()
{
  const uint8_t inactiveLevel = AppConfig::Alarm::relayActiveHigh ? LOW : HIGH;
  digitalWrite(AppConfig::Alarm::relayPin, inactiveLevel);
  pinMode(AppConfig::Alarm::relayPin, OUTPUT);
  ready_ = true;
  Serial.printf(
    "Alarm relay ready on IO%d; inactive at startup.\n",
    AppConfig::Alarm::relayPin);
}

void AlarmApplication::update()
{
  if (!ready_) {
    return;
  }

  const uint32_t nowMs = millis();
  if (relayActive_
      && static_cast<int32_t>(nowMs - relayOffAt_) >= 0) {
    setRelay(false);
    Serial.println("Alarm relay duration ended; relay switched off.");
  }

  if (!clock_.ready()) {
    return;
  }

  const ClockDateTime now = clock_.dateTime();
  const uint32_t today = dateKey(now);
  const AlarmSettings &settings = settings_.alarms();

  for (uint8_t index = 0; index < settings.count; ++index) {
    const AlarmItemSettings &alarm = settings.items[index];
    const uint32_t signature = configurationSignature(alarm);
    if (!configurationInitialized_[index]
        || configurationSignatures_[index] != signature) {
      configurationSignatures_[index] = signature;
      configurationInitialized_[index] = true;
      lastTriggeredDate_[index] = 0;
    }

    if (alarm.hour != now.hour
        || alarm.minute != now.minute
        || lastTriggeredDate_[index] == today
        || !isScheduledForDay(alarm, now)) {
      continue;
    }

    // Store the date before activating the output so this alarm cannot fire
    // repeatedly during the matching minute.
    lastTriggeredDate_[index] = today;
    trigger(index, alarm);
  }

  for (uint8_t index = settings.count; index < maximumAlarmCount; ++index) {
    configurationInitialized_[index] = false;
    lastTriggeredDate_[index] = 0;
  }
}

bool AlarmApplication::relayActive() const
{
  return relayActive_;
}

uint32_t AlarmApplication::dateKey(const ClockDateTime &dateTime)
{
  return static_cast<uint32_t>(dateTime.year) * 10000UL
      + static_cast<uint32_t>(dateTime.month) * 100UL
      + dateTime.day;
}

uint32_t AlarmApplication::configurationSignature(
  const AlarmItemSettings &alarm)
{
  return static_cast<uint32_t>(alarm.hour)
      | (static_cast<uint32_t>(alarm.minute) << 5)
      | (static_cast<uint32_t>(alarm.durationSeconds) << 11)
      | (static_cast<uint32_t>(alarm.dayMode) << 23)
      | (static_cast<uint32_t>(alarm.customDayMask) << 25);
}

bool AlarmApplication::isWeekday(const ClockDateTime &dateTime)
{
  const DateTime date(
    dateTime.year,
    dateTime.month,
    dateTime.day,
    dateTime.hour,
    dateTime.minute,
    dateTime.second);
  const uint8_t dayOfWeek = date.dayOfTheWeek();
  return dayOfWeek >= 1 && dayOfWeek <= 5;
}

bool AlarmApplication::isScheduledForDay(
  const AlarmItemSettings &alarm,
  const ClockDateTime &dateTime)
{
  if (alarm.dayMode == ScheduleDayMode::everyDay) {
    return true;
  }
  if (alarm.dayMode == ScheduleDayMode::weekdays) {
    return isWeekday(dateTime);
  }

  const DateTime date(
    dateTime.year,
    dateTime.month,
    dateTime.day,
    dateTime.hour,
    dateTime.minute,
    dateTime.second);
  const uint8_t rtcDay = date.dayOfTheWeek();
  const uint8_t mondayBasedDay = rtcDay == 0 ? 6 : rtcDay - 1;
  return (alarm.customDayMask & (1U << mondayBasedDay)) != 0;
}

void AlarmApplication::trigger(
  uint8_t alarmIndex,
  const AlarmItemSettings &alarm)
{
  const uint32_t nowMs = millis();
  const uint32_t requestedOffAt =
    nowMs + static_cast<uint32_t>(alarm.durationSeconds) * 1000UL;

  if (!relayActive_
      || static_cast<int32_t>(requestedOffAt - relayOffAt_) > 0) {
    relayOffAt_ = requestedOffAt;
  }
  setRelay(true);

  Serial.printf(
    "Alarm %u triggered at %02u:%02u for %u seconds.\n",
    static_cast<unsigned>(alarmIndex + 1),
    static_cast<unsigned>(alarm.hour),
    static_cast<unsigned>(alarm.minute),
    static_cast<unsigned>(alarm.durationSeconds));
}

void AlarmApplication::setRelay(bool active)
{
  const uint8_t activeLevel = AppConfig::Alarm::relayActiveHigh ? HIGH : LOW;
  digitalWrite(
    AppConfig::Alarm::relayPin,
    active ? activeLevel : static_cast<uint8_t>(!activeLevel));
  relayActive_ = active;
}


