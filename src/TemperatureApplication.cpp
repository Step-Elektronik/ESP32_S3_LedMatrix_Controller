#include "TemperatureApplication.h"

#include <Arduino.h>
#include <Wire.h>

#include "AppConfig.h"

namespace {

constexpr uint32_t measurementIntervalMs = 2000;
constexpr uint32_t detectionRetryIntervalMs = 10000;
constexpr uint32_t softResetRecoveryMs = 2;

} // namespace

void TemperatureApplication::begin()
{
  // SHT40 and DS3231 share the same I2C bus.
  Wire.begin(AppConfig::i2cSdaPin, AppConfig::i2cSclPin);
  detectSensor(millis());
}

void TemperatureApplication::update()
{
  const uint32_t now = millis();
  if (!sensorFound_) {
    if (now - lastDetectionAttemptAt_ >= detectionRetryIntervalMs) {
      detectSensor(now);
    }
    return;
  }

  if (measurementPending_) {
    if (now - measurementStartedAt_
        < SHT40::highPrecisionMeasurementTimeMs) {
      return;
    }

    measurementPending_ = false;
    float temperature = 0.0F;
    float humidity = 0.0F;
    if (!sensor_.readMeasurement(temperature, humidity)) {
      markUnavailable(now);
      return;
    }

    temperatureC_ = temperature;
    relativeHumidity_ = humidity;
    measurementValid_ = true;
    lastMeasurementAt_ = now;
    ++revision_;
    return;
  }

  if (!measurementValid_
      && now - lastMeasurementAt_ < softResetRecoveryMs) {
    return;
  }

  if (!measurementValid_
      || now - lastMeasurementAt_ >= measurementIntervalMs) {
    if (!sensor_.startHighPrecisionMeasurement()) {
      markUnavailable(now);
      return;
    }
    measurementPending_ = true;
    measurementStartedAt_ = now;
  }
}

bool TemperatureApplication::sensorFound() const
{
  return sensorFound_;
}

bool TemperatureApplication::ready() const
{
  return sensorFound_ && measurementValid_;
}

float TemperatureApplication::temperatureC() const
{
  return temperatureC_;
}

uint32_t TemperatureApplication::revision() const
{
  return revision_;
}

bool TemperatureApplication::detectSensor(uint32_t now)
{
  lastDetectionAttemptAt_ = now;
  if (!sensor_.begin(Wire)) {
    Serial.println("WARNING: SHT40 not found; temperature will show TMP?.");
    return false;
  }

  if (!sensor_.softReset()) {
    markUnavailable(now);
    return false;
  }
  sensorFound_ = true;
  measurementValid_ = false;
  measurementPending_ = false;
  lastMeasurementAt_ = now;
  ++revision_;
  Serial.println("SHT40 detected on I2C address 0x44.");
  return true;
}

void TemperatureApplication::markUnavailable(uint32_t now)
{
  sensorFound_ = false;
  measurementPending_ = false;
  measurementValid_ = false;
  lastDetectionAttemptAt_ = now;
  ++revision_;
  Serial.println("WARNING: SHT40 communication lost; retry scheduled.");
}


