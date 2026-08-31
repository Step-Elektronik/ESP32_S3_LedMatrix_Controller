#include "NetworkStatusLedService.h"

#include <Arduino.h>

#include "AppConfig.h"
#include "WiFiApplication.h"

NetworkStatusLedService::NetworkStatusLedService(const WiFiApplication &wifi)
  : wifi_(wifi)
{
}

void NetworkStatusLedService::begin()
{
  pinMode(AppConfig::StatusLeds::accessPointClientPin, OUTPUT);
  pinMode(AppConfig::StatusLeds::stationConnectedPin, OUTPUT);
  digitalWrite(AppConfig::StatusLeds::accessPointClientPin, LOW);
  digitalWrite(AppConfig::StatusLeds::stationConnectedPin, LOW);
}

void NetworkStatusLedService::update()
{
  const uint32_t now = millis();
  updateLed(
    AppConfig::StatusLeds::accessPointClientPin,
    wifi_.accessPointClientCount() > 0,
    accessPointLed_,
    now);
  updateLed(
    AppConfig::StatusLeds::stationConnectedPin,
    wifi_.stationHasIpAddress(),
    stationLed_,
    now);
}

void NetworkStatusLedService::updateLed(
  int8_t pin,
  bool shouldBlink,
  LedRuntime &runtime,
  uint32_t now)
{
  if (!shouldBlink) {
    if (runtime.active || runtime.outputHigh) {
      digitalWrite(pin, LOW);
    }
    runtime = {};
    return;
  }

  if (!runtime.active) {
    runtime.active = true;
    runtime.outputHigh = true;
    runtime.lastToggleAt = now;
    digitalWrite(pin, HIGH);
    return;
  }

  const uint32_t elapsed = now - runtime.lastToggleAt;
  if (elapsed < AppConfig::StatusLeds::toggleIntervalMs) {
    return;
  }

  const uint32_t elapsedPeriods =
    elapsed / AppConfig::StatusLeds::toggleIntervalMs;
  runtime.lastToggleAt +=
    elapsedPeriods * AppConfig::StatusLeds::toggleIntervalMs;
  if ((elapsedPeriods & 1U) != 0) {
    runtime.outputHigh = !runtime.outputHigh;
    digitalWrite(pin, runtime.outputHigh ? HIGH : LOW);
  }
}


