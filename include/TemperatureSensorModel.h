#pragma once

#include <stdint.h>

enum class TemperatureSensorType : uint8_t {
  sht40 = 0,
  tmp112 = 1,
  hdc2022 = 2
};

// Resolution options shared by the HDC2022 temperature and humidity ADCs.
enum class Hdc2022Resolution : uint8_t {
  bits14 = 0,
  bits11 = 1,
  bits9 = 2
};

enum class Sht40Precision : uint8_t {
  high = 0,
  medium = 1,
  low = 2
};

// TMP112 continuous conversion rates (CR1:CR0).
enum class Tmp112ConversionRate : uint8_t {
  hz0_25 = 0,
  hz1 = 1,
  hz4 = 2,
  hz8 = 3
};

constexpr uint16_t minimumSensorIntervalSeconds = 1;
constexpr uint16_t maximumSensorIntervalSeconds = 60;
// Calibration offsets are stored in tenths: +-10.0 degC and +-20.0 %RH.
constexpr int16_t maximumTemperatureOffsetTenths = 100;
constexpr int16_t maximumHumidityOffsetTenths = 200;

struct TemperatureSensorSettings {
  TemperatureSensorType type = TemperatureSensorType::tmp112;
  uint16_t measurementIntervalSeconds = 2;
  int16_t temperatureOffsetTenths = 0;
  int16_t humidityOffsetTenths = 0;

  uint8_t sht40Address = 0x44;
  Sht40Precision sht40Precision = Sht40Precision::high;

  uint8_t tmp112Address = 0x48;
  Tmp112ConversionRate tmp112ConversionRate = Tmp112ConversionRate::hz4;
  bool tmp112ExtendedMode = false;

  uint8_t hdc2022Address = 0x40;
  Hdc2022Resolution hdc2022TemperatureResolution = Hdc2022Resolution::bits14;
  Hdc2022Resolution hdc2022HumidityResolution = Hdc2022Resolution::bits14;
};

constexpr bool temperatureSensorHasHumidity(TemperatureSensorType type)
{
  return type != TemperatureSensorType::tmp112;
}

constexpr const char *temperatureSensorName(TemperatureSensorType type)
{
  return type == TemperatureSensorType::sht40 ? "SHT40"
    : type == TemperatureSensorType::hdc2022 ? "HDC2022"
    : "TMP112";
}

// Shown on the panel instead of a value while the selected sensor is missing.
constexpr const char *temperatureSensorMissingText(TemperatureSensorType type)
{
  return type == TemperatureSensorType::sht40 ? "SHT?"
    : type == TemperatureSensorType::hdc2022 ? "HDC?"
    : "TMP?";
}
