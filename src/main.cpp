#include <Arduino.h>

#include "AlarmApplication.h"
#include "AppConfig.h"
#include "BootGuardService.h"
#include "BuzzerService.h"
#include "ClockApplication.h"
#include "CountdownApplication.h"
#include "FirmwareUpdateService.h"
#include "OnlineUpdateService.h"
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
CountdownApplication countdownApplication(
  settingsService,
  clockApplication,
  alarmApplication);
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
  countdownApplication,
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
OnlineUpdateService onlineUpdateService(wifiApplication, bootGuardService);
PortalApplication portalApplication(
  settingsService,
  panelDisplay,
  clockApplication,
  temperatureApplication,
  contentApplication,
  valueBoxApplication,
  countdownApplication,
  alarmApplication,
  panelSleepApplication,
  wifiApplication,
  timeSyncApplication,
  bootGuardService,
  firmwareUpdateService,
  onlineUpdateService);

// Keep a freshly installed firmware in "pending verify" until it has run
// for a while (BootGuardService confirms it); a crash before that makes the
// bootloader return to the previous firmware.
extern "C" bool verifyRollbackLater()
{
  return true;
}

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
  countdownApplication.begin();
  modbusRtuSlaveService.begin();
  contentApplication.begin();
  panelSleepApplication.begin();

  // Start networking only after blocking startup tests are complete so mobile
  // captive-portal probes always find a responsive HTTP service.
  wifiApplication.begin();
  timeSyncApplication.begin();
  onlineUpdateService.begin();
  portalApplication.begin();
}

void loop()
{
  watchdogService.feed();
  bootGuardService.update();
  wifiApplication.update();
  timeSyncApplication.update();
  onlineUpdateService.update();
  networkStatusLedService.update();
  portalApplication.update();
  clockApplication.update();
  alarmApplication.update();
  countdownApplication.update();
  valueBoxApplication.update();
  modbusRtuSlaveService.update();
  panelSleepApplication.update();
  temperatureApplication.update();
  contentApplication.update();
}


