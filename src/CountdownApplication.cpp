#include "CountdownApplication.h"

#include <Arduino.h>
#include <RTClib.h>

#include "AlarmApplication.h"
#include "ClockApplication.h"
#include "SettingsService.h"

namespace {

constexpr char preferencesNamespace[] = "countdown";
constexpr char sessionKey[] = "session";
constexpr uint8_t sessionVersion = 1;

struct StoredSession {
  uint8_t version;
  uint8_t state;
  uint32_t elapsedBeforeMs;
  // DS3231 time when the current run started; 0 if the RTC was unavailable.
  uint32_t runStartEpoch;
};

const char *stateName(CountdownState state)
{
  switch (state) {
    case CountdownState::running: return "running";
    case CountdownState::paused: return "paused";
    case CountdownState::finished: return "finished";
    case CountdownState::idle:
    default: return "idle";
  }
}

} // namespace

CountdownApplication::CountdownApplication(
  SettingsService &settings,
  ClockApplication &clock,
  AlarmApplication &alarm)
  : settings_(settings),
    clock_(clock),
    alarm_(alarm)
{
}

void CountdownApplication::begin()
{
  storageReady_ = preferences_.begin(preferencesNamespace, false);
  if (!storageReady_) {
    Serial.println("ERROR: Countdown storage could not be opened.");
  }
  restore();
}

void CountdownApplication::update()
{
  if (state_ != CountdownState::running) {
    return;
  }

  const uint32_t elapsed = elapsedMs();
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    if (!isCountdown(item) || alarmFired_[index]
        || elapsed < item.countdownSeconds * 1000UL) {
      continue;
    }
    alarmFired_[index] = true;
    Serial.printf("Countdown box %u reached zero.\n", static_cast<unsigned>(index + 1));
    alarm_.activateCountdownAlarm(item.countdownAlarmSeconds);
  }

  if (elapsed >= longestDurationSeconds() * 1000UL) {
    finish();
    return;
  }

  const uint32_t second = elapsed / 1000UL;
  if (second != lastElapsedSecond_) {
    lastElapsedSecond_ = second;
    ++revision_;
  }
}

bool CountdownApplication::start()
{
  if (!hasCountdownContent()) {
    return false;
  }
  if (state_ == CountdownState::running) {
    return true;
  }
  if (state_ == CountdownState::idle || state_ == CountdownState::finished) {
    alarm_.stopCountdownAlarm();
    elapsedBeforeMs_ = 0;
    for (bool &fired : alarmFired_) {
      fired = false;
    }
  }

  runStartedAt_ = millis();
  state_ = CountdownState::running;
  lastElapsedSecond_ = UINT32_MAX;
  ++revision_;
  persist();
  Serial.printf("Countdown started at %lu ms elapsed.\n", static_cast<unsigned long>(elapsedBeforeMs_));
  return true;
}

void CountdownApplication::pause()
{
  alarm_.stopCountdownAlarm();
  if (state_ != CountdownState::running) {
    return;
  }

  elapsedBeforeMs_ = elapsedMs();
  state_ = CountdownState::paused;
  ++revision_;
  persist();
  Serial.printf("Countdown paused at %lu ms elapsed.\n", static_cast<unsigned long>(elapsedBeforeMs_));
}

void CountdownApplication::reset()
{
  alarm_.stopCountdownAlarm();
  state_ = CountdownState::idle;
  elapsedBeforeMs_ = 0;
  for (bool &fired : alarmFired_) {
    fired = false;
  }
  lastElapsedSecond_ = UINT32_MAX;
  ++revision_;
  persist();
  Serial.println("Countdown reset.");
}

void CountdownApplication::reload()
{
  if (!hasCountdownContent()) {
    if (state_ != CountdownState::idle) {
      reset();
    }
    return;
  }

  // Boxes that are already past their time must not ring again; a box with
  // a longer, not yet reached duration still will.
  const uint32_t elapsed = elapsedMs();
  markExpiredBoxes(elapsed);
  if (state_ != CountdownState::idle
      && elapsed >= longestDurationSeconds() * 1000UL) {
    finish();
    return;
  }
  if (state_ == CountdownState::finished) {
    // A longer duration was configured after the time was up.
    elapsedBeforeMs_ = elapsed;
    state_ = CountdownState::paused;
    persist();
  }
  ++revision_;
}

CountdownState CountdownApplication::state() const
{
  return state_;
}

bool CountdownApplication::alarmActive() const
{
  return alarm_.countdownAlarmActive();
}

bool CountdownApplication::hasCountdownContent() const
{
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < content.count; ++index) {
    if (isCountdown(content.items[index])) {
      return true;
    }
  }
  return false;
}

uint32_t CountdownApplication::elapsedMs() const
{
  return state_ == CountdownState::running
    ? elapsedBeforeMs_ + (millis() - runStartedAt_)
    : elapsedBeforeMs_;
}

uint32_t CountdownApplication::remainingSeconds(const ContentItemSettings &item) const
{
  const uint32_t durationMs = item.countdownSeconds * 1000UL;
  const uint32_t elapsed = elapsedMs();
  if (elapsed >= durationMs) {
    return 0;
  }
  // Rounded up: the full duration shows at the start, zero only at the end.
  return (durationMs - elapsed + 999UL) / 1000UL;
}

void CountdownApplication::format(
  const ContentItemSettings &item,
  char *buffer,
  size_t bufferSize) const
{
  const unsigned long remaining = remainingSeconds(item);
  const unsigned long seconds = remaining % 60UL;
  // The configured duration decides which parts are shown, so the width
  // does not change while counting down.
  if (item.countdownSeconds >= 3600UL) {
    snprintf(
      buffer,
      bufferSize,
      "%lu:%02lu:%02lu",
      remaining / 3600UL,
      (remaining / 60UL) % 60UL,
      seconds);
  } else if (item.countdownSeconds >= 60UL) {
    snprintf(buffer, bufferSize, "%02lu:%02lu", remaining / 60UL, seconds);
  } else {
    snprintf(buffer, bufferSize, "%02lu", seconds);
  }
}

uint32_t CountdownApplication::revision() const
{
  return revision_;
}

bool CountdownApplication::isCountdown(const ContentItemSettings &item)
{
  return item.boxKind == ContentBoxKind::textBox
      && item.type == ContentType::countdown
      && !item.cycleEnabled;
}

uint32_t CountdownApplication::longestDurationSeconds() const
{
  uint32_t longest = 0;
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    if (isCountdown(item) && item.countdownSeconds > longest) {
      longest = item.countdownSeconds;
    }
  }
  return longest;
}

void CountdownApplication::markExpiredBoxes(uint32_t elapsed)
{
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    alarmFired_[index] = index < content.count
      && isCountdown(content.items[index])
      && elapsed >= content.items[index].countdownSeconds * 1000UL;
  }
}

void CountdownApplication::finish()
{
  elapsedBeforeMs_ = longestDurationSeconds() * 1000UL;
  state_ = CountdownState::finished;
  ++revision_;
  persist();
  Serial.println("Countdown finished.");
}

void CountdownApplication::persist()
{
  if (!storageReady_) {
    return;
  }

  StoredSession session = {};
  session.version = sessionVersion;
  session.state = static_cast<uint8_t>(state_);
  session.elapsedBeforeMs = elapsedBeforeMs_;
  uint32_t epoch = 0;
  if (state_ == CountdownState::running && rtcEpoch(epoch)) {
    // The run started at runStartedAt_; the RTC is read right after it.
    epoch -= (millis() - runStartedAt_) / 1000UL;
  }
  session.runStartEpoch = epoch;
  if (preferences_.putBytes(sessionKey, &session, sizeof(session)) != sizeof(session)) {
    Serial.println("ERROR: Countdown state could not be stored.");
  }
}

void CountdownApplication::restore()
{
  StoredSession session = {};
  if (!storageReady_
      || preferences_.getBytesLength(sessionKey) != sizeof(session)
      || preferences_.getBytes(sessionKey, &session, sizeof(session)) != sizeof(session)
      || session.version != sessionVersion
      || session.state > static_cast<uint8_t>(CountdownState::finished)
      || !hasCountdownContent()) {
    state_ = CountdownState::idle;
    elapsedBeforeMs_ = 0;
    return;
  }

  state_ = static_cast<CountdownState>(session.state);
  elapsedBeforeMs_ = session.elapsedBeforeMs;
  if (state_ == CountdownState::running) {
    uint32_t now = 0;
    if (session.runStartEpoch != 0 && rtcEpoch(now) && now >= session.runStartEpoch) {
      // Continue as if the power had never been off; boxes that reached
      // zero meanwhile do not ring afterwards.
      const uint32_t offSeconds = now - session.runStartEpoch;
      const uint32_t limitMs = longestDurationSeconds() * 1000UL;
      elapsedBeforeMs_ = offSeconds >= limitMs / 1000UL
        ? limitMs
        : elapsedBeforeMs_ + offSeconds * 1000UL;
      runStartedAt_ = millis();
      Serial.printf("Countdown resumed after restart, %lu s since start.\n", static_cast<unsigned long>(offSeconds));
    } else {
      // Without a valid RTC the time without power is unknown.
      state_ = CountdownState::paused;
      Serial.println("Countdown paused after restart; RTC time unavailable.");
    }
  }

  const uint32_t elapsed = elapsedMs();
  markExpiredBoxes(elapsed);
  if (state_ != CountdownState::idle
      && elapsed >= longestDurationSeconds() * 1000UL) {
    finish();
  }
  Serial.printf("Countdown state: %s.\n", stateName(state_));
  ++revision_;
}

bool CountdownApplication::rtcEpoch(uint32_t &epoch) const
{
  if (!clock_.ready()) {
    return false;
  }
  const ClockDateTime now = clock_.dateTime();
  const DateTime value(now.year, now.month, now.day, now.hour, now.minute, now.second);
  if (!value.isValid()) {
    return false;
  }
  epoch = value.unixtime();
  return true;
}
