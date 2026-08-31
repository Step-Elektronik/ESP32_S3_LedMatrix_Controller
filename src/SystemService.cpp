#include "SystemService.h"

#include <Arduino.h>
#include <esp_system.h>

#include "AppConfig.h"

namespace {

const char *resetReasonName(esp_reset_reason_t reason)
{
  switch (reason) {
    case ESP_RST_POWERON: return "power-on";
    case ESP_RST_EXT: return "external reset";
    case ESP_RST_SW: return "software restart";
    case ESP_RST_PANIC: return "software panic";
    case ESP_RST_INT_WDT: return "interrupt watchdog";
    case ESP_RST_TASK_WDT: return "task watchdog";
    case ESP_RST_WDT: return "watchdog";
    case ESP_RST_DEEPSLEEP: return "deep-sleep wake-up";
    case ESP_RST_BROWNOUT: return "brownout / low supply voltage";
    case ESP_RST_SDIO: return "SDIO reset";
    case ESP_RST_UNKNOWN:
    default: return "unknown";
  }
}

} // namespace

void SystemService::begin()
{
  Serial.begin(AppConfig::serialBaudRate);
  delay(250);
  const esp_reset_reason_t resetReason = esp_reset_reason();
  Serial.printf(
    "Reset reason: %d (%s)\n",
    static_cast<int>(resetReason),
    resetReasonName(resetReason));
}


