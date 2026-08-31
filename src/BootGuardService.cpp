#include "BootGuardService.h"

#include <Arduino.h>

#include "AppConfig.h"

namespace {

constexpr char preferencesNamespace[] = "boot-guard";
constexpr char failureCounterKey[] = "fail_count";

} // namespace

bool BootGuardService::begin()
{
  if (!preferences_.begin(preferencesNamespace, false)) {
    Serial.println("ERROR: Boot guard storage could not be opened.");
    return false;
  }

  uint8_t failureCount = preferences_.getUChar(failureCounterKey, 0);
  if (failureCount < UINT8_MAX) {
    ++failureCount;
  }

  if (preferences_.putUChar(failureCounterKey, failureCount) != 1) {
    Serial.println("ERROR: Boot guard counter could not be stored.");
    return false;
  }

  bootStartedAt_ = millis();
  ready_ = true;
  Serial.printf("Boot guard counter: %u/%u\n", failureCount, AppConfig::bootFailureLimit);

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
  if (preferences_.putUChar(failureCounterKey, 0) != 1) {
    Serial.println("ERROR: Boot guard counter could not be cleared.");
    return false;
  }

  counterCleared_ = true;
  return true;
}


