#include <Arduino.h>

#include "AlarmApplication.h"
#include "AppConfig.h"
#include "BootGuardService.h"
#include "BuzzerService.h"
#include "ClockApplication.h"
#include "FirmwareUpdateService.h"
#include "NetworkStatusLedService.h"
#include "ModbusRtuSlaveService.h"
#include "PanelDisplay.h"
#include "PanelContentApplication.h"
#include "PanelSleepApplication.h"
#include "PortalApplication.h"
#include "SettingsService.h"
#include "SystemService.h"
#include "TemperatureApplication.h"
#include "TimeSyncApplication.h"
#include "WatchdogService.h"
#include "ValueBoxApplication.h"
#include "WiFiApplication.h"

SystemService systemService;
WatchdogService watchdogService;
FirmwareUpdateService firmwareUpdateService(watchdogService);
BootGuardService bootGuardService;
BuzzerService buzzerService;
SettingsService settingsService;
PanelDisplay panelDisplay;
ClockApplication clockApplication;
AlarmApplication alarmApplication(clockApplication, settingsService);
TemperatureApplication temperatureApplication(settingsService);
ValueBoxApplication valueBoxApplication(settingsService);
ModbusRtuSlaveService modbusRtuSlaveService(
  valueBoxApplication,
  settingsService);
PanelContentApplication contentApplication(
  panelDisplay,
  clockApplication,
  temperatureApplication,
  valueBoxApplication,
  settingsService);
PanelSleepApplication panelSleepApplication(
  panelDisplay,
  clockApplication,
  settingsService);
WiFiApplication wifiApplication(settingsService);
NetworkStatusLedService networkStatusLedService(wifiApplication);
TimeSyncApplication timeSyncApplication(
  clockApplication,
  settingsService,
  wifiApplication);
PortalApplication portalApplication(
  settingsService,
  panelDisplay,
  clockApplication,
  temperatureApplication,
  contentApplication,
  valueBoxApplication,
  panelSleepApplication,
  wifiApplication,
  timeSyncApplication,
  bootGuardService,
  firmwareUpdateService);

void setup()
{
  systemService.begin();
  watchdogService.begin(AppConfig::watchdogTimeoutMs);
  bootGuardService.begin();
  settingsService.begin();

  if (bootGuardService.factoryResetRequired()) {
    if (settingsService.factoryReset()) {
      Serial.println("Factory settings restored successfully.");
    } else {
      Serial.println("ERROR: Factory settings could not be stored.");
    }
  }

  alarmApplication.begin();
  buzzerService.begin();
  networkStatusLedService.begin();
  if (panelDisplay.begin(settingsService.panel())) {
    panelDisplay.runStartupColorTest(AppConfig::startupColorDurationMs);
  }
  clockApplication.begin();
  temperatureApplication.begin();
  valueBoxApplication.begin();
  modbusRtuSlaveService.begin();
  contentApplication.begin();
  panelSleepApplication.begin();

  // Start networking only after blocking startup tests are complete so mobile
  // captive-portal probes always find a responsive HTTP service.
  wifiApplication.begin();
  timeSyncApplication.begin();
  portalApplication.begin();
}

void loop()
{
  watchdogService.feed();
  bootGuardService.update();
  wifiApplication.update();
  timeSyncApplication.update();
  networkStatusLedService.update();
  portalApplication.update();
  clockApplication.update();
  alarmApplication.update();
  valueBoxApplication.update();
  modbusRtuSlaveService.update();
  panelSleepApplication.update();
  temperatureApplication.update();
  contentApplication.update();
}


