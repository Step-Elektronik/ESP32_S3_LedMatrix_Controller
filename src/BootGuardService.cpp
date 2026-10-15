#include "BootGuardService.h"

#include <Arduino.h>
#include <esp_system.h>

#include "AppConfig.h"

namespace {

constexpr char preferencesNamespace[] = "boot-guard";
constexpr char failureCounterKey[] = "fail_count";

// The reset reason tells how the previous boot ended. Only crashes, watchdog
// resets and brownouts mean that boot was unstable; switching the power off,
// pressing reset or a planned software restart breaks the failure chain.
bool previousBootFailed(esp_reset_reason_t reason)
{
  switch (reason) {
    case ESP_RST_PANIC:
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT:
    case ESP_RST_BROWNOUT:
      return true;
    default:
      return false;
  }
}

} // namespace

bool BootGuardService::begin()
{
  if (!preferences_.begin(preferencesNamespace, false)) {
    Serial.println("ERROR: Boot guard storage could not be opened.");
    return false;
  }

  const uint8_t storedCount = preferences_.getUChar(failureCounterKey, 0);
  const esp_reset_reason_t resetReason = esp_reset_reason();
  uint8_t failureCount = 0;
  if (previousBootFailed(resetReason)) {
    failureCount = storedCount < UINT8_MAX ? storedCount + 1 : storedCount;
  }

  // Count this boot as unstable until it runs long enough; a crash before
  // that point leaves the value in place for the next boot to evaluate.
  if (failureCount != storedCount
      && preferences_.putUChar(failureCounterKey, failureCount) != 1) {
    Serial.println("ERROR: Boot guard counter could not be stored.");
    return false;
  }

  bootStartedAt_ = millis();
  ready_ = true;
  counterCleared_ = failureCount == 0;
  Serial.printf(
    "Boot guard: %u consecutive abnormal reset(s), limit %u.\n",
    failureCount,
    AppConfig::bootFailureLimit);

  if (failureCount > AppConfig::bootFailureLimit) {
    factoryResetRequired_ = true;
    resetCounter();
    Serial.println("Boot loop detected; factory settings will be restored.");
  }

  return true;
}

void BootGuardService::update()
{
  if (!ready_ || counterCleared_) {
    return;
  }

  if (millis() - bootStartedAt_ >= AppConfig::bootStableDurationMs) {
    if (resetCounter()) {
      Serial.println("Boot guard counter cleared after 60 seconds of stable operation.");
    }
  }
}

void BootGuardService::prepareForPlannedRestart()
{
  if (ready_) {
    resetCounter();
  }
}

bool BootGuardService::factoryResetRequired() const
{
  return factoryResetRequired_;
}

bool BootGuardService::resetCounter()
{
  if (preferences_.getUChar(failureCounterKey, 0) != 0
      && preferences_.putUChar(failureCounterKey, 0) != 1) {
    Serial.println("ERROR: Boot guard counter could not be cleared.");
    return false;
  }

  counterCleared_ = true;
  return true;
}
