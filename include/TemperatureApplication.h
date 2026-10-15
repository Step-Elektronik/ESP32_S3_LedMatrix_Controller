#pragma once

#include <HDC2022.h>
#include <SHT40.h>
#include <TMP112.h>
#include <stdint.h>

#include "TemperatureSensorModel.h"

class SettingsService;

// Reads the temperature (and humidity, if supported) from the sensor model
// selected in the settings: SHT40, TMP112 or HDC2022 on the shared I2C bus.
class TemperatureApplication {
public:
  explicit TemperatureApplication(SettingsService &settings);

  void begin();
  void update();
  // Applies changed sensor settings without a restart.
  void reload();

  TemperatureSensorType sensorType() const;
  const char *sensorName() const;
  // "TMP?", "SHT?" or "HDC?" for the panel while the sensor is missing.
  const char *missingText() const;
  bool humiditySupported() const;
  uint8_t sensorAddress() const;

  bool sensorFound() const;
  bool ready() const;
  float temperatureC() const;
  float relativeHumidity() const;
  uint32_t revision() const;

private:
  bool detectSensor(uint32_t now);
  bool startMeasurement();
  uint32_t measurementDurationMs() const;
  bool readMeasurement(float &temperatureC, float &relativeHumidity);
  void handleFailure(uint32_t now);
  void markUnavailable(uint32_t now);

  SettingsService &settings_;
  TemperatureSensorSettings config_;
  SHT40 sht40_;
  TMP112 tmp112_;
  HDC2022 hdc2022_;

  float temperatureC_ = 0.0F;
  float relativeHumidity_ = 0.0F;
  uint32_t measurementStartedAt_ = 0;
  uint32_t lastMeasurementAt_ = 0;
  uint32_t lastDetectionAttemptAt_ = 0;
  uint32_t readyAt_ = 0;
  uint32_t revision_ = 0;
  uint8_t consecutiveFailures_ = 0;
  bool sensorFound_ = false;
  bool measurementPending_ = false;
  bool measurementValid_ = false;
};
