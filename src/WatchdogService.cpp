#include "WatchdogService.h"

#include <Arduino.h>
#include <esp_err.h>
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>

bool WatchdogService::begin(uint32_t timeoutMs)
{
  esp_task_wdt_config_t config = {};
  config.timeout_ms = timeoutMs;
  config.idle_core_mask = (1U << portNUM_PROCESSORS) - 1U;
  config.trigger_panic = true;

  esp_err_t result = esp_task_wdt_init(&config);
  if (result == ESP_ERR_INVALID_STATE) {
    result = esp_task_wdt_reconfigure(&config);
  }

  if (result != ESP_OK) {
    Serial.printf(
      "ERROR: Watchdog configuration failed: %s\n",
      esp_err_to_name(result)
    );
    return false;
  }

  const esp_err_t taskStatus = esp_task_wdt_status(nullptr);
  if (taskStatus == ESP_ERR_NOT_FOUND) {
    result = esp_task_wdt_add(nullptr);
  } else {
    result = taskStatus;
  }

  if (result != ESP_OK) {
    Serial.printf(
      "ERROR: Loop task could not be added to watchdog: %s\n",
      esp_err_to_name(result)
    );
    return false;
  }

  active_ = true;
  feed();
  Serial.printf("Watchdog enabled: %lu ms\n", static_cast<unsigned long>(timeoutMs));
  return true;
}

void WatchdogService::feed()
{
  if (active_) {
    esp_task_wdt_reset();
  }
}


