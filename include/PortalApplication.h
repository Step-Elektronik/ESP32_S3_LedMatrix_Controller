#pragma once

#include <WebServer.h>
#include <stdint.h>

struct SchoolBellSettings;

class AlarmApplication;
class ClockApplication;
class CountdownApplication;
class BootGuardService;
class FirmwareUpdateService;
class OnlineUpdateService;
class PanelContentApplication;
class PanelDisplay;
class PanelSleepApplication;
class SettingsService;
class TemperatureApplication;
class TimeSyncApplication;
class ValueBoxApplication;
class WiFiApplication;

class PortalApplication {
public:
  PortalApplication(
    SettingsService &settings,
    PanelDisplay &panel,
    ClockApplication &clock,
    TemperatureApplication &temperature,
    PanelContentApplication &content,
    ValueBoxApplication &valueBoxes,
    CountdownApplication &countdown,
    AlarmApplication &alarm,
    PanelSleepApplication &panelSleep,
    WiFiApplication &wifi,
    TimeSyncApplication &timeSync,
    BootGuardService &bootGuard,
    FirmwareUpdateService &firmwareUpdate,
    OnlineUpdateService &onlineUpdate);

  void begin();
  void update();

private:
  using Handler = void (PortalApplication::*)();

  void configureRoutes();
  void onAuthorized(const char *uri, HTTPMethod method, Handler handler);
  bool requestFromStation();
  bool authorized();
  bool authorize();

  void sendHomePage();
  void sendSettingsPage();
  void sendI18nScript();
  void sendFirmwareScript();
  void sendCompressedPage(
    const uint8_t *content,
    size_t contentLength,
    PGM_P contentType = PSTR("text/html; charset=utf-8"));
  void sendPortalRedirect();
  void sendSettingsJson();
  void sendWifiStatusJson();
  void sendContentJson();
  void sendFontPreviewData();
  void sendTemperatureJson();
  void sendCountdownJson();
  void controlCountdown();

  void saveContentSettings();
  void savePanelSettings();
  void saveClockSettings();
  void saveSleepSettings();
  void saveTimeSyncSettings();
  void saveTemperatureSensorSettings();
  void saveAlarmSettings();
  bool parseSchoolBellSettings(SchoolBellSettings &school, String &error) const;
  void testAlarmRelay();
  void saveCommunicationSettings();
  void saveNetworkSettings();
  void startPanelColorTest();
  // Device manager: answered without login and with CORS, see
  // web/cihaz-yoneticisi.html.
  void sendDiscoveryJson();
  void startIdentify();
  void saveDeviceSettings();
  void endPanelTest();
  void handleFirmwareUpload();
  void finishFirmwareUploadRequest();
  void sendOnlineUpdateJson();
  void controlOnlineUpdate();
  String buildOnlineUpdateJson(const String &message = String()) const;

  String buildSettingsJson();
  String buildWifiStatusJson() const;
  String buildContentJson() const;
  String buildTemperatureJson() const;
  String buildCountdownJson(const String &message = String()) const;
  void appendSensorJson(String &json) const;
  void appendSchoolBellJson(String &json) const;

  void sendJson(
    bool success,
    const String &message,
    int statusCode = 200);
  void scheduleRestart();

  bool postValue(const char *name, String &value) const;
  static String escapeJson(const String &value);

  SettingsService &settings_;
  PanelDisplay &panel_;
  ClockApplication &clock_;
  TemperatureApplication &temperature_;
  PanelContentApplication &content_;
  ValueBoxApplication &valueBoxes_;
  CountdownApplication &countdown_;
  AlarmApplication &alarm_;
  PanelSleepApplication &panelSleep_;
  WiFiApplication &wifi_;
  TimeSyncApplication &timeSync_;
  BootGuardService &bootGuard_;
  FirmwareUpdateService &firmwareUpdate_;
  OnlineUpdateService &onlineUpdate_;
  WebServer webServer_;
  bool colorTestActive_ = false;
  uint32_t colorTestStartedAt_ = 0;
  // Identify flashes the whole panel so the device can be found in a room.
  bool identifyActive_ = false;
  uint32_t identifyStartedAt_ = 0;
  uint8_t identifyPhase_ = UINT8_MAX;
  bool restartPending_ = false;
  uint32_t restartScheduledAt_ = 0;
  bool uploadAuthorized_ = false;
  bool uploadBlocked_ = false;
};
