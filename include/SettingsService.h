#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <stdint.h>

#include "AlarmModel.h"
#include "AppConfig.h"
#include "ContentModel.h"

enum class PanelConnectionStyle : uint8_t {
  topLeftDown = 1,
  topRightDown = 2,
  bottomLeftUp = 3,
  bottomRightUp = 4,
  topLeftDownZigzag = 5,
  topRightDownZigzag = 6,
  bottomRightUpZigzag = 7,
  bottomLeftUpZigzag = 8
};

struct PanelSettings {
  uint8_t scanDivisor = 4;
  uint8_t rows = 1;
  uint8_t columns = 1;
  uint8_t brightness = 64;
  PanelConnectionStyle connectionStyle = PanelConnectionStyle::topRightDown;

  uint16_t panelCount() const
  {
    return static_cast<uint16_t>(rows) * columns;
  }
};

struct NetworkSettings {
  String apSsid;
  String apPassword;
  String staSsid;
  String staPassword;
};

enum class UartParity : uint8_t {
  none = 0,
  even = 1,
  odd = 2
};

enum class UartHandshake : uint8_t {
  none = 0,
  rs485Direction = 1
};

struct CommunicationSettings {
  uint8_t slaveAddress = AppConfig::Modbus::defaultSlaveAddress;
  uint32_t baudRate = AppConfig::Modbus::defaultBaudRate;
  uint8_t dataBits = 8;
  UartParity parity = UartParity::none;
  UartHandshake handshake = UartHandshake::rs485Direction;
};

struct PanelSleepSettings {
  bool enabled = false;
  uint16_t startMinuteOfDay = 23 * 60;
  uint16_t endMinuteOfDay = 7 * 60;
  ScheduleDayMode dayMode = ScheduleDayMode::everyDay;
};

class SettingsService {
public:
  bool begin();

  const PanelSettings &panel() const;
  const NetworkSettings &network() const;
  const CommunicationSettings &communication() const;
  const PanelSleepSettings &sleep() const;
  const AlarmSettings &alarms() const;
  const ContentSettings &content() const;

  bool savePanel(const PanelSettings &settings);
  bool saveNetwork(const NetworkSettings &settings);
  bool saveCommunication(const CommunicationSettings &settings);
  bool saveSleep(const PanelSleepSettings &settings);
  bool saveAlarms(const AlarmSettings &settings);
  bool saveContent(const ContentSettings &settings);
  bool factoryReset();

  static bool validatePanel(const PanelSettings &settings, String &error);
  static bool validateNetwork(const NetworkSettings &settings, String &error);
  static bool validateCommunication(
    const CommunicationSettings &settings,
    String &error);
  static bool validateSleep(const PanelSleepSettings &settings, String &error);
  static bool validateAlarms(const AlarmSettings &settings, String &error);
  static bool validateContent(
    const ContentSettings &settings,
    const PanelSettings &panel,
    String &error);

private:
  void loadDefaults();

  Preferences preferences_;
  PanelSettings panel_;
  NetworkSettings network_;
  CommunicationSettings communication_;
  PanelSleepSettings sleep_;
  AlarmSettings alarms_;
  ContentSettings content_;
  bool ready_ = false;
};

