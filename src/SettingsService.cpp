#include "SettingsService.h"

#include <cmath>
#include <cstring>

#include "AppConfig.h"

namespace {

constexpr char preferencesNamespace[] = "step-rgb";

// Schema v1 supported only every-day and weekday recurrence. Keep this exact
// layout so saved alarms can be upgraded to the per-day mask schema.
struct LegacyAlarmItemSettingsV1 {
  uint8_t hour;
  uint8_t minute;
  uint16_t durationSeconds;
  ScheduleDayMode dayMode;
};

struct LegacyAlarmSettingsV1 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyAlarmItemSettingsV1 items[maximumAlarmCount];
};

// Schema v1 did not contain per-TextBox rotation. Keep this exact layout so
// existing user designs can be upgraded instead of being discarded.
struct LegacyContentItemSettingsV1 {
  uint8_t panelIndex;
  ContentType type;
  int16_t x;
  int16_t y;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  bool scrolling;
  ScrollDirection direction;
  uint8_t speedPixelsPerSecond;
  char text[maxContentTextLength];
};

struct LegacyContentSettingsV1 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV1 items[maxContentItems];
};

// Schema v2 added rotation but did not yet contain the per-TextBox content
// cycle settings.
struct LegacyContentItemSettingsV2 {
  uint8_t panelIndex;
  ContentType type;
  TextRotation rotation;
  int16_t x;
  int16_t y;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  bool scrolling;
  ScrollDirection direction;
  uint8_t speedPixelsPerSecond;
  char text[maxContentTextLength];
};

struct LegacyContentSettingsV2 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV2 items[maxContentItems];
};

// Schema v3 added per-TextBox content cycling. Schema v4 introduced the
// TextBox/ValueBox distinction and the value-source configuration.
struct LegacyContentItemSettingsV3 {
  uint8_t panelIndex;
  ContentType type;
  TextRotation rotation;
  int16_t x;
  int16_t y;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  bool scrolling;
  ScrollDirection direction;
  uint8_t speedPixelsPerSecond;
  bool cycleEnabled;
  uint8_t cycleContentMask;
  uint16_t cycleIntervalSeconds;
  char text[maxContentTextLength];
};

struct LegacyContentSettingsV3 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV3 items[maxContentItems];
};

// Schema v5 adds the temperature display format. Preserve the exact schema v4
// layout so existing panel designs can be upgraded without being discarded.
struct LegacyContentItemSettingsV4 {
  ContentBoxKind boxKind;
  uint8_t panelIndex;
  ContentType type;
  TextRotation rotation;
  int16_t x;
  int16_t y;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  bool scrolling;
  ScrollDirection direction;
  uint8_t speedPixelsPerSecond;
  bool cycleEnabled;
  uint8_t cycleContentMask;
  uint16_t cycleIntervalSeconds;
  char text[maxContentTextLength];
  ValueSource valueSource;
  ModbusValueType modbusValueType;
  CounterDirection counterDirection;
  uint16_t registerAddress;
  int32_t initialIntegerValue;
  float initialFloatValue;
};

struct LegacyContentSettingsV4 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV4 items[maxContentItems];
};

// Schema v6 adds a font selection to every TextBox/ValueBox. Preserve the
// exact schema v5 layout so existing designs retain every previous setting.
struct LegacyContentItemSettingsV5 {
  ContentBoxKind boxKind;
  uint8_t panelIndex;
  ContentType type;
  TextRotation rotation;
  int16_t x;
  int16_t y;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  bool scrolling;
  ScrollDirection direction;
  uint8_t speedPixelsPerSecond;
  bool cycleEnabled;
  uint8_t cycleContentMask;
  uint16_t cycleIntervalSeconds;
  char text[maxContentTextLength];
  ValueSource valueSource;
  ModbusValueType modbusValueType;
  CounterDirection counterDirection;
  uint16_t registerAddress;
  int32_t initialIntegerValue;
  float initialFloatValue;
  TemperatureDisplayFormat temperatureDisplayFormat;
};

struct LegacyContentSettingsV5 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV5 items[maxContentItems];
};

bool isValidSsid(const String &ssid, bool allowEmpty)
{
  const size_t length = ssid.length();
  return (allowEmpty && length == 0) || (length >= 1 && length <= 32);
}

bool isValidPassword(const String &password, bool allowEmpty)
{
  const size_t length = password.length();
  return (allowEmpty && length == 0) || (length >= 8 && length <= 63);
}

bool storeString(Preferences &preferences, const char *key, const String &value)
{
  const size_t written = preferences.putString(key, value);
  if (!value.isEmpty()) {
    return written == value.length();
  }

  // Preferences::putString() returns zero for a successfully stored empty
  // string, so verify the value explicitly in that case.
  return preferences.isKey(key)
      && preferences.getString(key, String("__missing__")).isEmpty();
}

void initializeDefaultContent(ContentSettings &content)
{
  // Avoid a very large aggregate temporary here. Some ESP32-S2 toolchain
  // versions fail internally while lowering ContentSettings{}.
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = 1;

  ContentItemSettings &defaultClock = content.items[0];
  defaultClock.panelIndex = 0;
  defaultClock.type = ContentType::time;
  defaultClock.x = 1;
  defaultClock.y = 3;
  defaultClock.red = AppConfig::clockColorRed;
  defaultClock.green = AppConfig::clockColorGreen;
  defaultClock.blue = AppConfig::clockColorBlue;
  defaultClock.direction = ScrollDirection::left;
  defaultClock.speedPixelsPerSecond = 10;
  defaultClock.cycleContentMask = contentTypeMask(ContentType::time);
  defaultClock.cycleIntervalSeconds = defaultCycleIntervalSeconds;
  defaultClock.fontId = FontModel::defaultFontId;
}

void initializeDefaultAlarms(AlarmSettings &alarms)
{
  std::memset(&alarms, 0, sizeof(alarms));
  alarms.schemaVersion = alarmSchemaVersion;
}

void migrateLegacyAlarms(
  const LegacyAlarmSettingsV1 &legacy,
  AlarmSettings &alarms)
{
  std::memset(&alarms, 0, sizeof(alarms));
  alarms.schemaVersion = alarmSchemaVersion;
  alarms.count = legacy.count;

  const uint8_t itemCount = legacy.count > maximumAlarmCount
    ? maximumAlarmCount
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyAlarmItemSettingsV1 &source = legacy.items[index];
    AlarmItemSettings &target = alarms.items[index];
    target.hour = source.hour;
    target.minute = source.minute;
    target.durationSeconds = source.durationSeconds;
    target.dayMode = source.dayMode;
    target.customDayMask = source.dayMode == ScheduleDayMode::weekdays
      ? alarmWeekdaysMask
      : alarmAllDaysMask;
  }
}

void initializeCycleDefaults(ContentItemSettings &item)
{
  item.cycleEnabled = false;
  item.cycleContentMask = contentTypeMask(item.type);
  item.cycleIntervalSeconds = defaultCycleIntervalSeconds;
}

void initializeFontDefault(ContentItemSettings &item)
{
  item.fontId = FontModel::defaultFontId;
}

void migrateLegacyContent(
  const LegacyContentSettingsV1 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV1 &source = legacy.items[index];
    ContentItemSettings &target = content.items[index];
    target.panelIndex = source.panelIndex;
    target.type = source.type;
    target.rotation = TextRotation::degrees0;
    target.x = source.x;
    target.y = source.y;
    target.red = source.red;
    target.green = source.green;
    target.blue = source.blue;
    target.scrolling = source.scrolling;
    target.direction = source.direction;
    target.speedPixelsPerSecond = source.speedPixelsPerSecond;
    std::memcpy(target.text, source.text, sizeof(target.text));
    initializeCycleDefaults(target);
    initializeFontDefault(target);
  }
}

void migrateLegacyContent(
  const LegacyContentSettingsV2 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV2 &source = legacy.items[index];
    ContentItemSettings &target = content.items[index];
    target.panelIndex = source.panelIndex;
    target.type = source.type;
    target.rotation = source.rotation;
    target.x = source.x;
    target.y = source.y;
    target.red = source.red;
    target.green = source.green;
    target.blue = source.blue;
    target.scrolling = source.scrolling;
    target.direction = source.direction;
    target.speedPixelsPerSecond = source.speedPixelsPerSecond;
    std::memcpy(target.text, source.text, sizeof(target.text));
    initializeCycleDefaults(target);
    initializeFontDefault(target);
  }
}

void migrateLegacyContent(
  const LegacyContentSettingsV3 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV3 &source = legacy.items[index];
    ContentItemSettings &target = content.items[index];
    target.boxKind = ContentBoxKind::textBox;
    target.panelIndex = source.panelIndex;
    target.type = source.type;
    target.rotation = source.rotation;
    target.x = source.x;
    target.y = source.y;
    target.red = source.red;
    target.green = source.green;
    target.blue = source.blue;
    target.scrolling = source.scrolling;
    target.direction = source.direction;
    target.speedPixelsPerSecond = source.speedPixelsPerSecond;
    target.cycleEnabled = source.cycleEnabled;
    target.cycleContentMask = source.cycleContentMask;
    target.cycleIntervalSeconds = source.cycleIntervalSeconds;
    std::memcpy(target.text, source.text, sizeof(target.text));
    initializeFontDefault(target);
  }
}

void migrateLegacyContent(
  const LegacyContentSettingsV4 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV4 &source = legacy.items[index];
    ContentItemSettings &target = content.items[index];
    target.boxKind = source.boxKind;
    target.panelIndex = source.panelIndex;
    target.type = source.type;
    target.rotation = source.rotation;
    target.x = source.x;
    target.y = source.y;
    target.red = source.red;
    target.green = source.green;
    target.blue = source.blue;
    target.scrolling = source.scrolling;
    target.direction = source.direction;
    target.speedPixelsPerSecond = source.speedPixelsPerSecond;
    target.cycleEnabled = source.cycleEnabled;
    target.cycleContentMask = source.cycleContentMask;
    target.cycleIntervalSeconds = source.cycleIntervalSeconds;
    std::memcpy(target.text, source.text, sizeof(target.text));
    target.valueSource = source.valueSource;
    target.modbusValueType = source.modbusValueType;
    target.counterDirection = source.counterDirection;
    target.registerAddress = source.registerAddress;
    target.initialIntegerValue = source.initialIntegerValue;
    target.initialFloatValue = source.initialFloatValue;
    target.temperatureDisplayFormat = TemperatureDisplayFormat::decimal;
    initializeFontDefault(target);
  }
}

void migrateLegacyContent(
  const LegacyContentSettingsV5 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV5 &source = legacy.items[index];
    ContentItemSettings &target = content.items[index];
    target.boxKind = source.boxKind;
    target.panelIndex = source.panelIndex;
    target.type = source.type;
    target.rotation = source.rotation;
    target.x = source.x;
    target.y = source.y;
    target.red = source.red;
    target.green = source.green;
    target.blue = source.blue;
    target.scrolling = source.scrolling;
    target.direction = source.direction;
    target.speedPixelsPerSecond = source.speedPixelsPerSecond;
    target.cycleEnabled = source.cycleEnabled;
    target.cycleContentMask = source.cycleContentMask;
    target.cycleIntervalSeconds = source.cycleIntervalSeconds;
    std::memcpy(target.text, source.text, sizeof(target.text));
    target.valueSource = source.valueSource;
    target.modbusValueType = source.modbusValueType;
    target.counterDirection = source.counterDirection;
    target.registerAddress = source.registerAddress;
    target.initialIntegerValue = source.initialIntegerValue;
    target.initialFloatValue = source.initialFloatValue;
    target.temperatureDisplayFormat = source.temperatureDisplayFormat;
    initializeFontDefault(target);
  }
}

} // namespace

bool SettingsService::begin()
{
  loadDefaults();

  if (!preferences_.begin(preferencesNamespace, false)) {
    Serial.println("ERROR: Settings storage could not be opened; defaults are active.");
    return false;
  }

  panel_.scanDivisor = preferences_.getUChar("panel_scan", panel_.scanDivisor);
  panel_.rows = preferences_.getUChar("panel_rows", panel_.rows);
  panel_.columns = preferences_.getUChar("panel_cols", panel_.columns);
  panel_.brightness = preferences_.getUChar("panel_light", panel_.brightness);
  panel_.connectionStyle = static_cast<PanelConnectionStyle>(
    preferences_.getUChar(
      "panel_chain",
      static_cast<uint8_t>(panel_.connectionStyle)));

  network_.apSsid = preferences_.getString("ap_ssid", network_.apSsid);
  network_.apPassword = preferences_.getString("ap_pass", network_.apPassword);
  network_.staSsid = preferences_.getString("sta_ssid", network_.staSsid);
  network_.staPassword = preferences_.getString("sta_pass", network_.staPassword);

  communication_.baudRate = preferences_.getUInt(
    "uart_baud",
    communication_.baudRate);
  communication_.dataBits = preferences_.getUChar(
    "uart_bits",
    communication_.dataBits);
  communication_.parity = static_cast<UartParity>(preferences_.getUChar(
    "uart_parity",
    static_cast<uint8_t>(communication_.parity)));
  communication_.handshake = static_cast<UartHandshake>(preferences_.getUChar(
    "uart_handshake",
    static_cast<uint8_t>(communication_.handshake)));

  sleep_.enabled = preferences_.getBool("sleep_enabled", sleep_.enabled);
  sleep_.startMinuteOfDay = preferences_.getUShort(
    "sleep_start",
    sleep_.startMinuteOfDay);
  sleep_.endMinuteOfDay = preferences_.getUShort(
    "sleep_end",
    sleep_.endMinuteOfDay);
  sleep_.dayMode = static_cast<ScheduleDayMode>(preferences_.getUChar(
    "sleep_days",
    static_cast<uint8_t>(sleep_.dayMode)));

  bool alarmSettingsRepaired = false;
  const size_t storedAlarmSize = preferences_.getBytesLength("alarms");
  if (storedAlarmSize == sizeof(AlarmSettings)) {
    if (preferences_.getBytes("alarms", &alarms_, sizeof(alarms_))
        != sizeof(alarms_)) {
      initializeDefaultAlarms(alarms_);
      alarmSettingsRepaired = true;
    } else if (alarms_.schemaVersion == 1
               && storedAlarmSize == sizeof(LegacyAlarmSettingsV1)) {
      LegacyAlarmSettingsV1 legacy;
      std::memset(&legacy, 0, sizeof(legacy));
      preferences_.getBytes("alarms", &legacy, sizeof(legacy));
      migrateLegacyAlarms(legacy, alarms_);
      Serial.println("Alarm settings upgraded from schema v1 to custom-day schema v2.");
      alarmSettingsRepaired = true;
    }
  } else if (storedAlarmSize == sizeof(LegacyAlarmSettingsV1)) {
    LegacyAlarmSettingsV1 legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    preferences_.getBytes("alarms", &legacy, sizeof(legacy));
    if (legacy.schemaVersion == 1) {
      migrateLegacyAlarms(legacy, alarms_);
      Serial.println("Alarm settings upgraded from schema v1 to custom-day schema v2.");
    }
    alarmSettingsRepaired = true;
  } else {
    alarmSettingsRepaired = true;
  }

  bool contentSettingsRepaired = false;
  const size_t storedContentSize = preferences_.getBytesLength("content");
  uint16_t storedContentVersion = 0;
  if (storedContentSize > 0 && storedContentSize <= sizeof(ContentSettings)) {
    ContentSettings storedContentProbe;
    std::memset(&storedContentProbe, 0, sizeof(storedContentProbe));
    if (preferences_.getBytes(
          "content",
          &storedContentProbe,
          sizeof(storedContentProbe)) == storedContentSize) {
      storedContentVersion = storedContentProbe.schemaVersion;
    }
  }

  if (storedContentVersion == contentSchemaVersion
      && storedContentSize == sizeof(ContentSettings)) {
    if (preferences_.getBytes("content", &content_, sizeof(content_))
        != sizeof(content_)) {
      contentSettingsRepaired = true;
    }
  } else if (storedContentVersion == 5
             && storedContentSize == sizeof(LegacyContentSettingsV5)) {
    LegacyContentSettingsV5 legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    preferences_.getBytes("content", &legacy, sizeof(legacy));
    migrateLegacyContent(legacy, content_);
    Serial.println("Content settings upgraded from schema v5 to font schema v6.");
    contentSettingsRepaired = true;
  } else if (storedContentVersion == 4
             && storedContentSize == sizeof(LegacyContentSettingsV4)) {
    LegacyContentSettingsV4 legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    preferences_.getBytes("content", &legacy, sizeof(legacy));
    migrateLegacyContent(legacy, content_);
    Serial.println("Content settings upgraded from schema v4 to font schema v6.");
    contentSettingsRepaired = true;
  } else if (storedContentVersion == 3
             && storedContentSize == sizeof(LegacyContentSettingsV3)) {
    LegacyContentSettingsV3 legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    preferences_.getBytes("content", &legacy, sizeof(legacy));
    migrateLegacyContent(legacy, content_);
    Serial.println("Content settings upgraded from schema v3 to font schema v6.");
    contentSettingsRepaired = true;
  } else if (storedContentVersion == 2
             && storedContentSize == sizeof(LegacyContentSettingsV2)) {
    LegacyContentSettingsV2 legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    preferences_.getBytes("content", &legacy, sizeof(legacy));
    migrateLegacyContent(legacy, content_);
    Serial.println("Content settings upgraded from schema v2 to font schema v6.");
    contentSettingsRepaired = true;
  } else if (storedContentVersion == 1
             && storedContentSize == sizeof(LegacyContentSettingsV1)) {
    LegacyContentSettingsV1 legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    preferences_.getBytes("content", &legacy, sizeof(legacy));
    migrateLegacyContent(legacy, content_);
    Serial.println("Content settings upgraded from schema v1 to font schema v6.");
    contentSettingsRepaired = true;
  } else {
    contentSettingsRepaired = true;
  }

  bool panelSettingsRepaired = false;
  bool networkSettingsRepaired = false;
  bool communicationSettingsRepaired = false;
  bool sleepSettingsRepaired = false;
  String validationError;
  if (!validatePanel(panel_, validationError)) {
    Serial.printf("Invalid stored panel settings: %s; defaults restored.\n", validationError.c_str());
    panel_ = PanelSettings{
      PANEL_SCAN_DIVISOR,
      AppConfig::panelRows,
      AppConfig::panelColumns,
      AppConfig::panelBrightness,
      PanelConnectionStyle::topRightDown
    };
    panelSettingsRepaired = true;
  }

  if (!validateNetwork(network_, validationError)) {
    Serial.printf("Invalid stored network settings: %s; defaults restored.\n", validationError.c_str());
    network_ = NetworkSettings{
      AppConfig::Network::apSsid,
      AppConfig::Network::apPassword,
      String(),
      String()
    };
    networkSettingsRepaired = true;
  }

  if (!validateCommunication(communication_, validationError)) {
    Serial.printf(
      "Invalid stored communication settings: %s; defaults restored.\n",
      validationError.c_str());
    communication_ = CommunicationSettings{};
    communicationSettingsRepaired = true;
  }

  if (!validateSleep(sleep_, validationError)) {
    Serial.printf("Invalid stored sleep settings: %s; defaults restored.\n", validationError.c_str());
    sleep_ = PanelSleepSettings{};
    sleepSettingsRepaired = true;
  }

  if (!validateAlarms(alarms_, validationError)) {
    Serial.printf("Invalid stored alarm settings: %s; defaults restored.\n", validationError.c_str());
    initializeDefaultAlarms(alarms_);
    alarmSettingsRepaired = true;
  }

  if (!validateContent(content_, panel_, validationError)) {
    Serial.printf("Invalid stored content settings: %s; defaults restored.\n", validationError.c_str());
    contentSettingsRepaired = true;
    initializeDefaultContent(content_);
  }

  ready_ = true;

  // Repair invalid NVS values immediately so the next boot does not load the
  // same unsafe configuration again.
  if (panelSettingsRepaired && !savePanel(panel_)) {
    Serial.println("ERROR: Safe panel defaults could not be stored.");
  }
  if (networkSettingsRepaired && !saveNetwork(network_)) {
    Serial.println("ERROR: Safe network defaults could not be stored.");
  }
  if (communicationSettingsRepaired
      && !saveCommunication(communication_)) {
    Serial.println("ERROR: Safe communication defaults could not be stored.");
  }
  if (sleepSettingsRepaired && !saveSleep(sleep_)) {
    Serial.println("ERROR: Safe sleep defaults could not be stored.");
  }
  if (alarmSettingsRepaired && !saveAlarms(alarms_)) {
    Serial.println("ERROR: Safe alarm defaults could not be stored.");
  }
  if (contentSettingsRepaired && !saveContent(content_)) {
    Serial.println("ERROR: Safe content defaults could not be stored.");
  }
  return true;
}

const PanelSettings &SettingsService::panel() const
{
  return panel_;
}

const NetworkSettings &SettingsService::network() const
{
  return network_;
}

const CommunicationSettings &SettingsService::communication() const
{
  return communication_;
}

const PanelSleepSettings &SettingsService::sleep() const
{
  return sleep_;
}

const AlarmSettings &SettingsService::alarms() const
{
  return alarms_;
}

const ContentSettings &SettingsService::content() const
{
  return content_;
}

bool SettingsService::savePanel(const PanelSettings &settings)
{
  String error;
  if (!ready_ || !validatePanel(settings, error)) {
    return false;
  }

  const bool stored =
    preferences_.putUChar("panel_scan", settings.scanDivisor) == 1
    && preferences_.putUChar("panel_rows", settings.rows) == 1
    && preferences_.putUChar("panel_cols", settings.columns) == 1
    && preferences_.putUChar("panel_light", settings.brightness) == 1
    && preferences_.putUChar(
      "panel_chain",
      static_cast<uint8_t>(settings.connectionStyle)) == 1;
  if (stored) {
    panel_ = settings;
  }
  return stored;
}

bool SettingsService::saveNetwork(const NetworkSettings &settings)
{
  String error;
  if (!ready_ || !validateNetwork(settings, error)) {
    return false;
  }

  bool stored = storeString(preferences_, "ap_ssid", settings.apSsid);
  stored = storeString(preferences_, "ap_pass", settings.apPassword) && stored;
  stored = storeString(preferences_, "sta_ssid", settings.staSsid) && stored;
  stored = storeString(preferences_, "sta_pass", settings.staPassword) && stored;
  if (stored) {
    network_ = settings;
  }
  return stored;
}

bool SettingsService::saveCommunication(
  const CommunicationSettings &settings)
{
  String error;
  if (!ready_ || !validateCommunication(settings, error)) {
    return false;
  }

  const bool stored =
    preferences_.putUInt("uart_baud", settings.baudRate) == 4
    && preferences_.putUChar("uart_bits", settings.dataBits) == 1
    && preferences_.putUChar(
      "uart_parity",
      static_cast<uint8_t>(settings.parity)) == 1
    && preferences_.putUChar(
      "uart_handshake",
      static_cast<uint8_t>(settings.handshake)) == 1;
  if (stored) {
    communication_ = settings;
  }
  return stored;
}

bool SettingsService::saveSleep(const PanelSleepSettings &settings)
{
  String error;
  if (!ready_ || !validateSleep(settings, error)) {
    return false;
  }

  const bool stored =
    preferences_.putBool("sleep_enabled", settings.enabled) == 1
    && preferences_.putUShort("sleep_start", settings.startMinuteOfDay) == 2
    && preferences_.putUShort("sleep_end", settings.endMinuteOfDay) == 2
    && preferences_.putUChar(
      "sleep_days",
      static_cast<uint8_t>(settings.dayMode)) == 1;
  if (stored) {
    sleep_ = settings;
  }
  return stored;
}

bool SettingsService::saveAlarms(const AlarmSettings &settings)
{
  String error;
  if (!ready_ || !validateAlarms(settings, error)) {
    return false;
  }

  if (preferences_.putBytes("alarms", &settings, sizeof(settings))
      != sizeof(settings)) {
    return false;
  }

  alarms_ = settings;
  return true;
}

bool SettingsService::saveContent(const ContentSettings &settings)
{
  String error;
  if (!ready_ || !validateContent(settings, panel_, error)) {
    return false;
  }

  if (preferences_.putBytes("content", &settings, sizeof(settings))
      != sizeof(settings)) {
    return false;
  }

  content_ = settings;
  return true;
}

bool SettingsService::factoryReset()
{
  if (!ready_) {
    return false;
  }

  const bool cleared = preferences_.clear();
  loadDefaults();
  if (!cleared) {
    return false;
  }

  const bool panelStored = savePanel(panel_);
  const bool networkStored = saveNetwork(network_);
  const bool communicationStored = saveCommunication(communication_);
  const bool sleepStored = saveSleep(sleep_);
  const bool alarmsStored = saveAlarms(alarms_);
  const bool contentStored = saveContent(content_);
  return panelStored
      && networkStored
      && communicationStored
      && sleepStored
      && alarmsStored
      && contentStored;
}

bool SettingsService::validatePanel(const PanelSettings &settings, String &error)
{
  if (settings.scanDivisor != 4 && settings.scanDivisor != 8) {
    error = "Tarama orani 1/4 veya 1/8 olmalidir.";
    return false;
  }
  if (settings.rows < 1 || settings.rows > AppConfig::maxPanelRows) {
    error = "Satir sayisi gecersiz.";
    return false;
  }
  if (settings.columns < 1 || settings.columns > AppConfig::maxPanelColumns) {
    error = "Sutun sayisi gecersiz.";
    return false;
  }
  if (settings.panelCount() > AppConfig::maxPanelCount) {
    error = "Toplam panel sayisi izin verilen siniri asiyor.";
    return false;
  }
  const uint8_t connectionStyle = static_cast<uint8_t>(settings.connectionStyle);
  if (connectionStyle < static_cast<uint8_t>(PanelConnectionStyle::topLeftDown)
      || connectionStyle > static_cast<uint8_t>(PanelConnectionStyle::bottomLeftUpZigzag)) {
    error = "Panel baglanti sekli gecersiz.";
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateNetwork(const NetworkSettings &settings, String &error)
{
  if (!isValidSsid(settings.apSsid, false)) {
    error = "AP SSID 1-32 karakter olmalidir.";
    return false;
  }
  if (!isValidPassword(settings.apPassword, false)) {
    error = "AP parolasi 8-63 karakter olmalidir.";
    return false;
  }
  if (!isValidSsid(settings.staSsid, true)) {
    error = "STA SSID en fazla 32 karakter olabilir.";
    return false;
  }
  if (!isValidPassword(settings.staPassword, true)) {
    error = "STA parolasi bos veya 8-63 karakter olmalidir.";
    return false;
  }
  if (settings.staSsid.isEmpty() && !settings.staPassword.isEmpty()) {
    error = "STA SSID bosken parola girilemez.";
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateCommunication(
  const CommunicationSettings &settings,
  String &error)
{
  const bool baudRateValid = settings.baudRate == 4800
    || settings.baudRate == 9600
    || settings.baudRate == 19200
    || settings.baudRate == 57600
    || settings.baudRate == 115200;
  if (!baudRateValid) {
    error = "UART baudrate gecersiz.";
    return false;
  }
  if (settings.dataBits < 5 || settings.dataBits > 8) {
    error = "UART data size 5-8 bit olmalidir.";
    return false;
  }
  if (static_cast<uint8_t>(settings.parity)
      > static_cast<uint8_t>(UartParity::odd)) {
    error = "UART parity ayari gecersiz.";
    return false;
  }
  if (static_cast<uint8_t>(settings.handshake)
      > static_cast<uint8_t>(UartHandshake::rs485Direction)) {
    error = "UART handshake ayari gecersiz.";
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateSleep(
  const PanelSleepSettings &settings,
  String &error)
{
  constexpr uint16_t minutesPerDay = 24 * 60;
  if (settings.startMinuteOfDay >= minutesPerDay
      || settings.endMinuteOfDay >= minutesPerDay) {
    error = "Uyku baslangic veya bitis saati gecersiz.";
    return false;
  }
  if (settings.startMinuteOfDay == settings.endMinuteOfDay) {
    error = "Uyku baslangic ve bitis saati farkli olmalidir.";
    return false;
  }
  if (static_cast<uint8_t>(settings.dayMode)
      > static_cast<uint8_t>(ScheduleDayMode::weekdays)) {
    error = "Uyku acilis gunu gecersiz.";
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateAlarms(
  const AlarmSettings &settings,
  String &error)
{
  if (settings.schemaVersion != alarmSchemaVersion
      || settings.count > maximumAlarmCount) {
    error = "Alarm veri formati gecersiz.";
    return false;
  }

  for (uint8_t index = 0; index < settings.count; ++index) {
    const AlarmItemSettings &alarm = settings.items[index];
    if (alarm.hour > 23 || alarm.minute > 59) {
      error = "Alarm saati gecersiz.";
      return false;
    }
    if (alarm.durationSeconds < minimumAlarmDurationSeconds
        || alarm.durationSeconds > maximumAlarmDurationSeconds) {
      error = "Alarm tetiklenme suresi 1-3600 saniye olmalidir.";
      return false;
    }
    if (static_cast<uint8_t>(alarm.dayMode)
        > static_cast<uint8_t>(ScheduleDayMode::customDays)) {
      error = "Alarm gun secimi gecersiz.";
      return false;
    }
    if ((alarm.customDayMask & ~alarmAllDaysMask) != 0
        || (alarm.dayMode == ScheduleDayMode::customDays
            && alarm.customDayMask == 0)) {
      error = "Alarm icin en az bir gecerli gun secilmelidir.";
      return false;
    }
  }

  error.clear();
  return true;
}

bool SettingsService::validateContent(
  const ContentSettings &settings,
  const PanelSettings &panel,
  String &error)
{
  if (settings.schemaVersion != contentSchemaVersion
      || settings.count > maxContentItems) {
    error = "Icerik formati gecersiz.";
    return false;
  }

  uint8_t panelItemCounts[AppConfig::maxPanelCount] = {};
  const int16_t displayWidth = AppConfig::panelWidth * panel.columns;
  const int16_t displayHeight = AppConfig::panelHeight * panel.rows;

  for (uint8_t index = 0; index < settings.count; ++index) {
    const ContentItemSettings &item = settings.items[index];
    if (static_cast<uint8_t>(item.boxKind)
        > static_cast<uint8_t>(ContentBoxKind::valueBox)) {
      error = "Icerik kutusu tipi gecersiz.";
      return false;
    }
    if (item.panelIndex >= panel.panelCount()) {
      error = "Icerik kutusu panel numarasi gecersiz.";
      return false;
    }
    if (++panelItemCounts[item.panelIndex] > maxBoxesPerPanel) {
      error = "Bir panelde toplam en fazla iki TextBox/ValueBox olabilir.";
      return false;
    }
    if (item.x < 0 || item.x >= displayWidth
        || item.y < 0 || item.y >= displayHeight) {
      error = "Icerik kutusu koordinati ekran disinda.";
      return false;
    }

    const uint8_t expectedPanel =
      (item.y / AppConfig::panelHeight) * panel.columns
      + (item.x / AppConfig::panelWidth);
    if (expectedPanel != item.panelIndex) {
      error = "Icerik kutusu koordinati ile panel numarasi uyusmuyor.";
      return false;
    }

    if (static_cast<uint8_t>(item.rotation)
        > static_cast<uint8_t>(TextRotation::degrees270)) {
      error = "Icerik kutusu dondurme acisi gecersiz.";
      return false;
    }
    if (item.fontId >= FontModel::fontCount) {
      error = "Icerik kutusu yazi tipi gecersiz.";
      return false;
    }
    if (item.boxKind == ContentBoxKind::textBox) {
      if (static_cast<uint8_t>(item.temperatureDisplayFormat)
          > static_cast<uint8_t>(TemperatureDisplayFormat::integer)) {
        error = "TextBox sicaklik gosterim tipi gecersiz.";
        return false;
      }
      const uint8_t type = static_cast<uint8_t>(item.type);
      if (type > static_cast<uint8_t>(ContentType::temperature)) {
        error = "TextBox veri tipi gecersiz.";
        return false;
      }
      if (std::memchr(item.text, '\0', sizeof(item.text)) == nullptr) {
        error = "TextBox metni sonlandirilmamis.";
        return false;
      }
      if (!item.cycleEnabled
          && item.type == ContentType::text
          && item.text[0] == '\0') {
        error = "Metin TextBox'u bos olamaz.";
        return false;
      }
      if ((item.cycleContentMask & ~allCycleContentMask) != 0) {
        error = "TextBox dongu icerigi gecersiz.";
        return false;
      }
      if (item.cycleEnabled) {
        if (item.cycleContentMask == 0) {
          error = "Dongu icin en az bir bilgi secilmelidir.";
          return false;
        }
        if (item.cycleIntervalSeconds < minimumCycleIntervalSeconds
            || item.cycleIntervalSeconds > maximumCycleIntervalSeconds) {
          error = "Dongu suresi 1-3600 saniye olmalidir.";
          return false;
        }
        if ((item.cycleContentMask & contentTypeMask(ContentType::text)) != 0
            && item.text[0] == '\0') {
          error = "Dongudeki yazi icerigi bos olamaz.";
          return false;
        }
      }
    } else {
      if (static_cast<uint8_t>(item.valueSource)
          > static_cast<uint8_t>(ValueSource::digitalInput)) {
        error = "ValueBox veri kaynagi gecersiz.";
        return false;
      }
      if (static_cast<uint8_t>(item.modbusValueType)
          > static_cast<uint8_t>(ModbusValueType::ieee754Float)) {
        error = "ValueBox Modbus veri tipi gecersiz.";
        return false;
      }
      if (static_cast<uint8_t>(item.counterDirection)
          > static_cast<uint8_t>(CounterDirection::down)) {
        error = "ValueBox sayac yonu gecersiz.";
        return false;
      }
      if (!std::isfinite(item.initialFloatValue)) {
        error = "ValueBox baslangic float degeri gecersiz.";
        return false;
      }
      if (item.valueSource == ValueSource::modbus) {
        if (item.modbusValueType == ModbusValueType::signedInteger16
            && (item.initialIntegerValue < INT16_MIN
                || item.initialIntegerValue > INT16_MAX)) {
          error = "Modbus integer baslangic degeri -32768 ile 32767 arasinda olmalidir.";
          return false;
        }
        if (item.modbusValueType == ModbusValueType::ieee754Float
            && item.registerAddress == UINT16_MAX) {
          error = "IEEE-754 float icin son register adresi kullanilamaz.";
          return false;
        }

        const uint32_t itemStart = item.registerAddress;
        const uint32_t itemEnd = itemStart
          + (item.modbusValueType == ModbusValueType::ieee754Float ? 1U : 0U);
        for (uint8_t previousIndex = 0; previousIndex < index; ++previousIndex) {
          const ContentItemSettings &previous = settings.items[previousIndex];
          if (previous.boxKind != ContentBoxKind::valueBox
              || previous.valueSource != ValueSource::modbus) {
            continue;
          }
          const uint32_t previousStart = previous.registerAddress;
          const uint32_t previousEnd = previousStart
            + (previous.modbusValueType == ModbusValueType::ieee754Float ? 1U : 0U);
          if (itemStart <= previousEnd && previousStart <= itemEnd) {
            error = "Modbus ValueBox register adresleri birbiriyle cakismamalidir.";
            return false;
          }
        }
      }
    }
    if (static_cast<uint8_t>(item.direction)
        > static_cast<uint8_t>(ScrollDirection::down)) {
      error = "Kaydirma yonu gecersiz.";
      return false;
    }
    if (item.speedPixelsPerSecond < 1 || item.speedPixelsPerSecond > 100) {
      error = "Kaydirma hizi 1-100 piksel/saniye olmalidir.";
      return false;
    }
  }

  error.clear();
  return true;
}

void SettingsService::loadDefaults()
{
  panel_ = PanelSettings{
    PANEL_SCAN_DIVISOR,
    AppConfig::panelRows,
    AppConfig::panelColumns,
    AppConfig::panelBrightness,
    PanelConnectionStyle::topRightDown
  };
  network_ = NetworkSettings{
    AppConfig::Network::apSsid,
    AppConfig::Network::apPassword,
    String(),
    String()
  };
  communication_ = CommunicationSettings{};
  sleep_ = PanelSleepSettings{};
  initializeDefaultAlarms(alarms_);
  initializeDefaultContent(content_);
}
