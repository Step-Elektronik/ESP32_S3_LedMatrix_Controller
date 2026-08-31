#pragma once

#include <SHT40.h>
#include <stdint.h>

class TemperatureApplication {
public:
  void begin();
  void update();

  bool sensorFound() const;
  bool ready() const;
  float temperatureC() const;
  uint32_t revision() const;

private:
  bool detectSensor(uint32_t now);
  void markUnavailable(uint32_t now);

  SHT40 sensor_;
  float temperatureC_ = 0.0F;
  float relativeHumidity_ = 0.0F;
  uint32_t measurementStartedAt_ = 0;
  uint32_t lastMeasurementAt_ = 0;
  uint32_t lastDetectionAttemptAt_ = 0;
  uint32_t revision_ = 0;
  bool sensorFound_ = false;
  bool measurementPending_ = false;
  bool measurementValid_ = false;
};


