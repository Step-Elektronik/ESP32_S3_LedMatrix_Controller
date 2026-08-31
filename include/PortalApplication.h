#pragma once

#include <WebServer.h>
#include <stdint.h>

class ClockApplication;
class BootGuardService;
class FirmwareUpdateService;
class PanelContentApplication;
class PanelDisplay;
class PanelSleepApplication;
class SettingsService;
class TemperatureApplication;
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
    PanelSleepApplication &panelSleep,
    WiFiApplication &wifi,
    BootGuardService &bootGuard,
    FirmwareUpdateService &firmwareUpdate);

  void begin();
  void update();

private:
  void configureRoutes();
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

  void saveContentSettings();
  void savePanelSettings();
  void saveClockSettings();
  void saveAlarmSettings();
  void saveCommunicationSettings();
  void saveNetworkSettings();
  void startPanelColorTest();
  void handleFirmwareUpload();
  void finishFirmwareUploadRequest();

  String buildSettingsJson() const;
  String buildWifiStatusJson() const;
  String buildContentJson() const;
  String buildTemperatureJson() const;

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
  PanelSleepApplication &panelSleep_;
  WiFiApplication &wifi_;
  BootGuardService &bootGuard_;
  FirmwareUpdateService &firmwareUpdate_;
  WebServer webServer_;
  bool colorTestActive_ = false;
  uint32_t colorTestStartedAt_ = 0;
  bool restartPending_ = false;
  uint32_t restartScheduledAt_ = 0;
};
