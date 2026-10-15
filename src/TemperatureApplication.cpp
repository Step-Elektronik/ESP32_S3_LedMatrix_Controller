#include "TemperatureApplication.h"

#include <Arduino.h>
#include <Wire.h>

#include "AppConfig.h"
#include "SettingsService.h"

namespace {

constexpr uint32_t detectionRetryIntervalMs = 10000;
// A single I2C glitch keeps the last value; repeated failures mean the
// sensor is gone and the panel switches to "TMP?", "SHT?" or "HDC?".
constexpr uint8_t maximumConsecutiveFailures = 3;

} // namespace

TemperatureApplication::TemperatureApplication(SettingsService &settings)
  : settings_(settings)
{
}

void TemperatureApplication::begin()
{
  // The temperature sensor and the DS3231 share the same I2C bus.
  Wire.begin(AppConfig::i2cSdaPin, AppConfig::i2cSclPin);
  reload();
}

void TemperatureApplication::reload()
{
  config_ = settings_.temperatureSensor();
  sensorFound_ = false;
  measurementPending_ = false;
  measurementValid_ = false;
  consecutiveFailures_ = 0;
  ++revision_;
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
    if (now - measurementStartedAt_ < measurementDurationMs()) {
      return;
    }

    measurementPending_ = false;
    float temperature = 0.0F;
    float humidity = 0.0F;
    if (!readMeasurement(temperature, humidity)) {
      handleFailure(now);
      return;
    }

    temperatureC_ = temperature + config_.temperatureOffsetTenths / 10.0F;
    relativeHumidity_ = humidity + config_.humidityOffsetTenths / 10.0F;
    if (relativeHumidity_ < 0.0F) {
      relativeHumidity_ = 0.0F;
    } else if (relativeHumidity_ > 100.0F) {
      relativeHumidity_ = 100.0F;
    }
    consecutiveFailures_ = 0;
    measurementValid_ = true;
    lastMeasurementAt_ = now;
    ++revision_;
    return;
  }

  if (static_cast<int32_t>(now - readyAt_) < 0) {
    return;
  }

  const uint32_t intervalMs =
    static_cast<uint32_t>(config_.measurementIntervalSeconds) * 1000U;
  if (!measurementValid_ || now - lastMeasurementAt_ >= intervalMs) {
    if (!startMeasurement()) {
      handleFailure(now);
      return;
    }
    measurementPending_ = true;
    measurementStartedAt_ = now;
  }
}

TemperatureSensorType TemperatureApplication::sensorType() const
{
  return config_.type;
}

const char *TemperatureApplication::sensorName() const
{
  return temperatureSensorName(config_.type);
}

const char *TemperatureApplication::missingText() const
{
  return temperatureSensorMissingText(config_.type);
}

bool TemperatureApplication::humiditySupported() const
{
  return temperatureSensorHasHumidity(config_.type);
}

uint8_t TemperatureApplication::sensorAddress() const
{
  switch (config_.type) {
    case TemperatureSensorType::sht40: return config_.sht40Address;
    case TemperatureSensorType::hdc2022: return config_.hdc2022Address;
    case TemperatureSensorType::tmp112:
    default: return config_.tmp112Address;
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

float TemperatureApplication::relativeHumidity() const
{
  return relativeHumidity_;
}

uint32_t TemperatureApplication::revision() const
{
  return revision_;
}

bool TemperatureApplication::detectSensor(uint32_t now)
{
  lastDetectionAttemptAt_ = now;
  bool detected = false;
  uint32_t settleMs = 0;

  switch (config_.type) {
    case TemperatureSensorType::sht40: {
      // The serial number read has CRC-protected data, so a different chip
      // at the same address is not mistaken for an SHT40.
      uint32_t serialNumber = 0;
      detected = sht40_.begin(Wire, config_.sht40Address)
        && sht40_.readSerialNumber(serialNumber)
        && sht40_.softReset();
      settleMs = SHT40::softResetTimeMs + 1;
      break;
    }
    case TemperatureSensorType::hdc2022:
      detected = hdc2022_.begin(Wire, config_.hdc2022Address)
        && hdc2022_.configure(
          static_cast<HDC2022::Resolution>(config_.hdc2022TemperatureResolution),
          static_cast<HDC2022::Resolution>(config_.hdc2022HumidityResolution));
      break;
    case TemperatureSensorType::tmp112:
    default:
      detected = tmp112_.begin(Wire, config_.tmp112Address)
        && tmp112_.configure(
          static_cast<TMP112::ConversionRate>(config_.tmp112ConversionRate),
          config_.tmp112ExtendedMode);
      // The register holds the previous format until the next conversion.
      settleMs = TMP112::conversionTimeMs;
      break;
  }

  if (!detected) {
    Serial.printf(
      "WARNING: %s not found at 0x%02X; panel shows %s.\n",
      sensorName(),
      sensorAddress(),
      missingText());
    return false;
  }

  sensorFound_ = true;
  measurementValid_ = false;
  measurementPending_ = false;
  consecutiveFailures_ = 0;
  readyAt_ = now + settleMs;
  ++revision_;
  Serial.printf("%s detected on I2C address 0x%02X.\n", sensorName(), sensorAddress());
  return true;
}

bool TemperatureApplication::startMeasurement()
{
  switch (config_.type) {
    case TemperatureSensorType::sht40:
      return sht40_.startMeasurement(
        static_cast<SHT40::Precision>(config_.sht40Precision));
    case TemperatureSensorType::hdc2022:
      return hdc2022_.startMeasurement();
    case TemperatureSensorType::tmp112:
    default:
      // Continuous conversion: the register always holds the latest result.
      return true;
  }
}

uint32_t TemperatureApplication::measurementDurationMs() const
{
  switch (config_.type) {
    case TemperatureSensorType::sht40:
      return SHT40::measurementTimeMs(
        static_cast<SHT40::Precision>(config_.sht40Precision));
    case TemperatureSensorType::hdc2022:
      return hdc2022_.measurementTimeMs();
    case TemperatureSensorType::tmp112:
    default:
      return 0;
  }
}

bool TemperatureApplication::readMeasurement(
  float &temperatureC,
  float &relativeHumidity)
{
  relativeHumidity = 0.0F;
  switch (config_.type) {
    case TemperatureSensorType::sht40:
      return sht40_.readMeasurement(temperatureC, relativeHumidity);
    case TemperatureSensorType::hdc2022:
      return hdc2022_.readMeasurement(temperatureC, relativeHumidity);
    case TemperatureSensorType::tmp112:
    default:
      return tmp112_.readTemperature(temperatureC);
  }
}

void TemperatureApplication::handleFailure(uint32_t now)
{
  measurementPending_ = false;
  if (++consecutiveFailures_ >= maximumConsecutiveFailures) {
    markUnavailable(now);
    return;
  }
  // Retry at the next interval with the last value still shown.
  lastMeasurementAt_ = now;
}

void TemperatureApplication::markUnavailable(uint32_t now)
{
  sensorFound_ = false;
  measurementPending_ = false;
  measurementValid_ = false;
  lastDetectionAttemptAt_ = now;
  ++revision_;
  Serial.printf(
    "WARNING: %s communication lost; panel shows %s, retry scheduled.\n",
    sensorName(),
    missingText());
}
