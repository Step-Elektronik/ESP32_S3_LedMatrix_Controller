#include "BuzzerService.h"

#include <Arduino.h>

#include "AppConfig.h"

void BuzzerService::begin()
{
  pinMode(AppConfig::Buzzer::pin, OUTPUT);
  digitalWrite(AppConfig::Buzzer::pin, LOW);

  bool outputHigh = false;
  for (uint8_t toggle = 0; toggle < AppConfig::Buzzer::startupToggleCount; ++toggle) {
    outputHigh = !outputHigh;
    digitalWrite(AppConfig::Buzzer::pin, outputHigh ? HIGH : LOW);
    delay(AppConfig::Buzzer::startupToggleIntervalMs);
  }

  digitalWrite(AppConfig::Buzzer::pin, LOW);
}


