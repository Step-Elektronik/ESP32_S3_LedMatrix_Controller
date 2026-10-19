#include "AlarmApplication.h"

#include <Arduino.h>
#include <RTClib.h>

#include "AppConfig.h"
#include "CalendarDate.h"
#include "ClockApplication.h"
#include "SchoolBellSchedule.h"
#include "SettingsService.h"

namespace {

const char *schoolBellKindName(SchoolBellKind kind)
{
  switch (kind) {
    case SchoolBellKind::lesson: return "lesson";
    case SchoolBellKind::breakTime: return "break";
    case SchoolBellKind::studentCall: return "student call";
    case SchoolBellKind::remembrance: return "remembrance";
    default: return "bell";
  }
}

} // namespace

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
  if (scheduledRelayActive_
      && static_cast<int32_t>(nowMs - scheduledRelayOffAt_) >= 0) {
    scheduledRelayActive_ = false;
    updateRelayOutput();
    Serial.println("Alarm relay duration ended.");
  }
  if (countdownRelayActive_
      && static_cast<int32_t>(nowMs - countdownRelayOffAt_) >= 0) {
    countdownRelayActive_ = false;
    updateRelayOutput();
    Serial.println("Countdown alarm duration ended.");
  }

  if (!clock_.ready()) {
    return;
  }

  const ClockDateTime now = clock_.dateTime();
  if (settings_.alarmMode() == AlarmMode::schoolBell) {
    updateSchoolBell(now);
  } else {
    updateManualAlarms(now);
  }
}

void AlarmApplication::updateManualAlarms(const ClockDateTime &now)
{
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

void AlarmApplication::updateSchoolBell(const ClockDateTime &now)
{
  const int32_t day = daysFromCivil(CalendarDate{now.year, now.month, now.day});
  const uint16_t minute = static_cast<uint16_t>(now.hour * 60U + now.minute);
  if (day == lastSchoolBellDay_ && minute == lastSchoolBellMinute_) {
    return;
  }
  lastSchoolBellDay_ = day;
  lastSchoolBellMinute_ = minute;

  const SchoolBellSettings &school = settings_.schoolBell();
  SchoolBellEvent event;
  if ((day == lastRungSchoolBellDay_ && minute == lastRungSchoolBellMinute_)
      || !SchoolBellSchedule::bellAt(school, day, minute, event)) {
    return;
  }

  lastRungSchoolBellDay_ = day;
  lastRungSchoolBellMinute_ = minute;
  activateScheduledRelay(school.durationSeconds);
  Serial.printf(
    "School bell (%s, lesson %u) at %02u:%02u for %u seconds.\n",
    schoolBellKindName(event.kind),
    static_cast<unsigned>(event.lesson + 1),
    static_cast<unsigned>(now.hour),
    static_cast<unsigned>(now.minute),
    static_cast<unsigned>(school.durationSeconds));
}

void AlarmApplication::testRelay(uint16_t durationSeconds)
{
  if (!ready_ || durationSeconds == 0) {
    return;
  }
  activateScheduledRelay(durationSeconds);
  Serial.printf("Relay test for %u seconds.\n", static_cast<unsigned>(durationSeconds));
}

bool AlarmApplication::relayActive() const
{
  return relayOutputActive_;
}

void AlarmApplication::activateCountdownAlarm(uint16_t durationSeconds)
{
  if (!ready_ || durationSeconds == 0) {
    return;
  }

  const uint32_t requestedOffAt =
    millis() + static_cast<uint32_t>(durationSeconds) * 1000UL;
  if (!countdownRelayActive_
      || static_cast<int32_t>(requestedOffAt - countdownRelayOffAt_) > 0) {
    countdownRelayOffAt_ = requestedOffAt;
  }
  countdownRelayActive_ = true;
  updateRelayOutput();
  Serial.printf("Countdown finished; alarm relay on for %u seconds.\n", durationSeconds);
}

void AlarmApplication::stopCountdownAlarm()
{
  if (!countdownRelayActive_) {
    return;
  }
  countdownRelayActive_ = false;
  updateRelayOutput();
  Serial.println("Countdown alarm stopped.");
}

bool AlarmApplication::countdownAlarmActive() const
{
  return countdownRelayActive_;
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
  activateScheduledRelay(alarm.durationSeconds);
  Serial.printf(
    "Alarm %u triggered at %02u:%02u for %u seconds.\n",
    static_cast<unsigned>(alarmIndex + 1),
    static_cast<unsigned>(alarm.hour),
    static_cast<unsigned>(alarm.minute),
    static_cast<unsigned>(alarm.durationSeconds));
}

void AlarmApplication::activateScheduledRelay(uint16_t durationSeconds)
{
  // Overlapping bells keep the relay on until the later one ends.
  const uint32_t requestedOffAt =
    millis() + static_cast<uint32_t>(durationSeconds) * 1000UL;
  if (!scheduledRelayActive_
      || static_cast<int32_t>(requestedOffAt - scheduledRelayOffAt_) > 0) {
    scheduledRelayOffAt_ = requestedOffAt;
  }
  scheduledRelayActive_ = true;
  updateRelayOutput();
}

void AlarmApplication::updateRelayOutput()
{
  const bool active = scheduledRelayActive_ || countdownRelayActive_;
  const uint8_t activeLevel = AppConfig::Alarm::relayActiveHigh ? HIGH : LOW;
  digitalWrite(
    AppConfig::Alarm::relayPin,
    active ? activeLevel : static_cast<uint8_t>(!activeLevel));
  relayOutputActive_ = active;
}


