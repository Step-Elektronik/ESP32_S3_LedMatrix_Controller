#pragma once

#include <stdint.h>

class WiFiApplication;

class NetworkStatusLedService {
public:
  explicit NetworkStatusLedService(const WiFiApplication &wifi);

  void begin();
  void update();

private:
  struct LedRuntime {
    bool active = false;
    bool outputHigh = false;
    uint32_t lastToggleAt = 0;
  };

  void updateLed(
    int8_t pin,
    bool shouldBlink,
    LedRuntime &runtime,
    uint32_t now);

  const WiFiApplication &wifi_;
  LedRuntime accessPointLed_;
  LedRuntime stationLed_;
};


