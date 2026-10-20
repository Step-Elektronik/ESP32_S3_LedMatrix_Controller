#include "SettingsService.h"

#include <cmath>
#include <cstring>
#include <memory>
#include <new>

#include "AppConfig.h"
#include "CalendarDate.h"
#include "DeviceIdentity.h"
#include "UiText.h"

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

// Schema v7 adds the clipping/scroll area and the ValueBox decimal places.
// The ValueBox unit suffix reuses the existing text field.
struct LegacyContentItemSettingsV6 {
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
  uint8_t fontId;
};

struct LegacyContentSettingsV6 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV6 items[maxContentItems];
};

// Schema v8 adds the countdown duration and its alarm time.
struct LegacyContentItemSettingsV7 {
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
  uint8_t fontId;
  ScrollArea scrollArea;
  uint8_t valueDecimals;
};

struct LegacyContentSettingsV7 {
  uint16_t schemaVersion;
  uint8_t count;
  LegacyContentItemSettingsV7 items[maxContentItems];
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

bool containsOnly(const String &value, const char *allowedPunctuation)
{
  for (size_t index = 0; index < value.length(); ++index) {
    const char character = value[index];
    const bool alphanumeric = (character >= 'a' && character <= 'z')
      || (character >= 'A' && character <= 'Z')
      || (character >= '0' && character <= '9');
    if (!alphanumeric && std::strchr(allowedPunctuation, character) == nullptr) {
      return false;
    }
  }
  return true;
}

bool isValidNtpServer(const String &server)
{
  return server.length() >= 1 && server.length() <= 63
      && containsOnly(server, ".-:");
}

bool isValidTimeZone(const String &timeZone)
{
  // POSIX TZ strings such as "<+03>-3" or "CET-1CEST,M3.5.0,M10.5.0/3".
  return timeZone.length() >= 1 && timeZone.length() <= 63
      && containsOnly(timeZone, "<>+-,./:");
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

bool isValidDateKey(uint32_t key)
{
  return isValidDate(
    static_cast<uint16_t>(key / 10000UL),
    static_cast<uint8_t>((key / 100UL) % 100UL),
    static_cast<uint8_t>(key % 100UL));
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

void applyContentV7Defaults(ContentItemSettings &item)
{
  // Designs created before schema v7 wrapped scrolling text around the whole
  // display and showed float ValueBoxes with one decimal place.
  item.scrollArea = ScrollArea::display;
  item.valueDecimals = item.boxKind == ContentBoxKind::valueBox
      && item.valueSource == ValueSource::modbus
      && item.modbusValueType == ModbusValueType::ieee754Float
    ? 1
    : 0;
  if (item.boxKind == ContentBoxKind::valueBox) {
    item.text[0] = '\0';
  }
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
    applyContentV7Defaults(target);
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
    applyContentV7Defaults(target);
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
    applyContentV7Defaults(target);
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
    applyContentV7Defaults(target);
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
    applyContentV7Defaults(target);
  }
}

void migrateLegacyContent(
  const LegacyContentSettingsV6 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV6 &source = legacy.items[index];
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
    target.fontId = source.fontId;
    applyContentV7Defaults(target);
  }
}

void migrateLegacyContent(
  const LegacyContentSettingsV7 &legacy,
  ContentSettings &content)
{
  std::memset(&content, 0, sizeof(content));
  content.schemaVersion = contentSchemaVersion;
  content.count = legacy.count;

  const uint8_t itemCount = legacy.count > maxContentItems
    ? maxContentItems
    : legacy.count;
  for (uint8_t index = 0; index < itemCount; ++index) {
    const LegacyContentItemSettingsV7 &source = legacy.items[index];
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
    target.fontId = source.fontId;
    target.scrollArea = source.scrollArea;
    target.valueDecimals = source.valueDecimals;
  }
}

// Copies a stored blob into a heap-allocated legacy layout and upgrades it.
// The loop task stack is only 8 KB, so the ~2.7 KB layouts stay off it.
template <typename LegacySettings>
bool migrateStoredContent(
  const uint8_t *blob,
  size_t blobSize,
  ContentSettings &content)
{
  if (blobSize != sizeof(LegacySettings)) {
    return false;
  }
  std::unique_ptr<LegacySettings> legacy(new (std::nothrow) LegacySettings);
  if (!legacy) {
    return false;
  }
  std::memcpy(legacy.get(), blob, sizeof(LegacySettings));
  migrateLegacyContent(*legacy, content);
  for (ContentItemSettings &item : content.items) {
    item.countdownSeconds = defaultCountdownSeconds;
    item.countdownAlarmSeconds = defaultCountdownAlarmSeconds;
  }
  return true;
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
  panel_.type = static_cast<PanelType>(
    preferences_.getUChar("panel_type", static_cast<uint8_t>(panel_.type)));
  panel_.hub12DataInverted = preferences_.getBool("hub12_data_inv", panel_.hub12DataInverted);
  panel_.hub12OeInverted = preferences_.getBool("hub12_oe_inv", panel_.hub12OeInverted);

  network_.apSsid = preferences_.getString("ap_ssid", network_.apSsid);
  network_.apPassword = preferences_.getString("ap_pass", network_.apPassword);
  network_.staSsid = preferences_.getString("sta_ssid", network_.staSsid);
  network_.staPassword = preferences_.getString("sta_pass", network_.staPassword);
  network_.stationPortalAuthentication = preferences_.getBool(
    "sta_auth",
    network_.stationPortalAuthentication);

  communication_.baudRate = preferences_.getUInt(
    "uart_baud",
    communication_.baudRate);
  communication_.slaveAddress = preferences_.getUChar(
    "modbus_id",
    communication_.slaveAddress);
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

  bool schoolBellRepaired = !loadSchoolBell();
  const uint8_t storedAlarmMode = preferences_.getUChar(
    "alarm_mode",
    static_cast<uint8_t>(AlarmMode::manual));
  const bool alarmModeRepaired =
    storedAlarmMode > static_cast<uint8_t>(AlarmMode::schoolBell);
  alarmMode_ = alarmModeRepaired
    ? AlarmMode::manual
    : static_cast<AlarmMode>(storedAlarmMode);

  // A damaged name falls back to the MAC based default.
  deviceName_ = preferences_.getString("dev_name", "");
  String deviceNameError;
  if (!validateDeviceName(deviceName_, deviceNameError)) {
    deviceName_ = "";
  }

  timeSync_.ntpEnabled = preferences_.getBool("ntp_enabled", timeSync_.ntpEnabled);
  timeSync_.ntpServer = preferences_.getString("ntp_server", timeSync_.ntpServer);
  timeSync_.timeZone = preferences_.getString("ntp_tz", timeSync_.timeZone);

  bool temperatureSensorRepaired = !loadTemperatureSensor();

  bool contentSettingsRepaired = !loadContent();

  bool panelSettingsRepaired = false;
  bool networkSettingsRepaired = false;
  bool communicationSettingsRepaired = false;
  bool sleepSettingsRepaired = false;
  bool timeSyncSettingsRepaired = false;
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

  if (!validateTimeSync(timeSync_, validationError)) {
    Serial.printf("Invalid stored time sync settings: %s; defaults restored.\n", validationError.c_str());
    timeSync_ = TimeSyncSettings{
      false,
      AppConfig::TimeSync::defaultNtpServer,
      AppConfig::TimeSync::defaultTimeZone
    };
    timeSyncSettingsRepaired = true;
  }

  if (!validateTemperatureSensor(temperatureSensor_, validationError)) {
    Serial.printf("Invalid stored temperature sensor settings: %s; defaults restored.\n", validationError.c_str());
    temperatureSensor_ = TemperatureSensorSettings{};
    temperatureSensorRepaired = true;
  }

  if (!validateAlarms(alarms_, validationError)) {
    Serial.printf("Invalid stored alarm settings: %s; defaults restored.\n", validationError.c_str());
    initializeDefaultAlarms(alarms_);
    alarmSettingsRepaired = true;
  }

  if (!validateSchoolBell(schoolBell_, validationError)) {
    Serial.printf("Invalid stored school bell settings: %s; defaults restored.\n", validationError.c_str());
    schoolBell_ = SchoolBellSettings{};
    schoolBellRepaired = true;
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
  if (timeSyncSettingsRepaired && !saveTimeSync(timeSync_)) {
    Serial.println("ERROR: Safe time sync defaults could not be stored.");
  }
  if (temperatureSensorRepaired && !saveTemperatureSensor(temperatureSensor_)) {
    Serial.println("ERROR: Temperature sensor settings could not be stored.");
  }
  if (alarmSettingsRepaired && !saveAlarms(alarms_)) {
    Serial.println("ERROR: Safe alarm defaults could not be stored.");
  }
  if (schoolBellRepaired && !saveSchoolBell(schoolBell_)) {
    Serial.println("ERROR: Safe school bell defaults could not be stored.");
  }
  if (alarmModeRepaired && !saveAlarmMode(alarmMode_)) {
    Serial.println("ERROR: Alarm mode could not be stored.");
  }
  if (contentSettingsRepaired && !saveContent(content_)) {
    Serial.println("ERROR: Safe content defaults could not be stored.");
  }
  return true;
}

bool SettingsService::loadTemperatureSensor()
{
  // Returns false when the settings did not exist yet and must be stored.
  if (!preferences_.isKey("ts_type")) {
    // Earlier firmware supported only the SHT40 at 0x44. Devices that already
    // have a stored design keep it; new devices start with the TMP112.
    temperatureSensor_ = TemperatureSensorSettings{};
    if (preferences_.isKey("content")) {
      temperatureSensor_.type = TemperatureSensorType::sht40;
    }
    return false;
  }

  TemperatureSensorSettings &sensor = temperatureSensor_;
  sensor.type = static_cast<TemperatureSensorType>(
    preferences_.getUChar("ts_type", static_cast<uint8_t>(sensor.type)));
  sensor.measurementIntervalSeconds =
    preferences_.getUShort("ts_interval", sensor.measurementIntervalSeconds);
  sensor.temperatureOffsetTenths =
    preferences_.getShort("ts_t_offset", sensor.temperatureOffsetTenths);
  sensor.humidityOffsetTenths =
    preferences_.getShort("ts_h_offset", sensor.humidityOffsetTenths);
  sensor.sht40Address = preferences_.getUChar("ts_sht_addr", sensor.sht40Address);
  sensor.sht40Precision = static_cast<Sht40Precision>(preferences_.getUChar(
    "ts_sht_prec",
    static_cast<uint8_t>(sensor.sht40Precision)));
  sensor.tmp112Address = preferences_.getUChar("ts_tmp_addr", sensor.tmp112Address);
  sensor.tmp112ConversionRate = static_cast<Tmp112ConversionRate>(preferences_.getUChar(
    "ts_tmp_rate",
    static_cast<uint8_t>(sensor.tmp112ConversionRate)));
  sensor.tmp112ExtendedMode = preferences_.getBool("ts_tmp_ext", sensor.tmp112ExtendedMode);
  sensor.hdc2022Address = preferences_.getUChar("ts_hdc_addr", sensor.hdc2022Address);
  sensor.hdc2022TemperatureResolution = static_cast<Hdc2022Resolution>(preferences_.getUChar(
    "ts_hdc_tres",
    static_cast<uint8_t>(sensor.hdc2022TemperatureResolution)));
  sensor.hdc2022HumidityResolution = static_cast<Hdc2022Resolution>(preferences_.getUChar(
    "ts_hdc_hres",
    static_cast<uint8_t>(sensor.hdc2022HumidityResolution)));
  return true;
}

bool SettingsService::loadSchoolBell()
{
  // Returns false when stored data is unusable and defaults must be stored.
  schoolBell_ = SchoolBellSettings{};
  if (!preferences_.isKey("school_bell")) {
    return true;
  }

  SchoolBellSettings stored;
  if (preferences_.getBytesLength("school_bell") != sizeof(stored)
      || preferences_.getBytes("school_bell", &stored, sizeof(stored)) != sizeof(stored)
      || stored.schemaVersion != schoolBellSchemaVersion) {
    Serial.println("Stored school bell settings are unreadable; defaults restored.");
    return false;
  }
  schoolBell_ = stored;
  return true;
}

bool SettingsService::loadContent()
{
  // Returns true when the stored design is current and can be used as-is;
  // false when it was migrated or replaced and must be stored again.
  const size_t storedSize = preferences_.getBytesLength("content");
  if (storedSize < sizeof(uint16_t) || storedSize > sizeof(ContentSettings)) {
    return false;
  }

  std::unique_ptr<uint8_t[]> blob(new (std::nothrow) uint8_t[storedSize]);
  if (!blob
      || preferences_.getBytes("content", blob.get(), storedSize) != storedSize) {
    return false;
  }

  uint16_t storedVersion = 0;
  std::memcpy(&storedVersion, blob.get(), sizeof(storedVersion));
  if (storedVersion == contentSchemaVersion) {
    if (storedSize != sizeof(ContentSettings)) {
      return false;
    }
    std::memcpy(&content_, blob.get(), sizeof(content_));
    return true;
  }

  bool migrated = false;
  switch (storedVersion) {
    case 7:
      migrated = migrateStoredContent<LegacyContentSettingsV7>(blob.get(), storedSize, content_);
      break;
    case 6:
      migrated = migrateStoredContent<LegacyContentSettingsV6>(blob.get(), storedSize, content_);
      break;
    case 5:
      migrated = migrateStoredContent<LegacyContentSettingsV5>(blob.get(), storedSize, content_);
      break;
    case 4:
      migrated = migrateStoredContent<LegacyContentSettingsV4>(blob.get(), storedSize, content_);
      break;
    case 3:
      migrated = migrateStoredContent<LegacyContentSettingsV3>(blob.get(), storedSize, content_);
      break;
    case 2:
      migrated = migrateStoredContent<LegacyContentSettingsV2>(blob.get(), storedSize, content_);
      break;
    case 1:
      migrated = migrateStoredContent<LegacyContentSettingsV1>(blob.get(), storedSize, content_);
      break;
    default:
      break;
  }
  if (migrated) {
    Serial.printf(
      "Content settings upgraded from schema v%u to schema v%u.\n",
      storedVersion,
      contentSchemaVersion);
  } else {
    initializeDefaultContent(content_);
  }
  return false;
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

const TimeSyncSettings &SettingsService::timeSync() const
{
  return timeSync_;
}

const TemperatureSensorSettings &SettingsService::temperatureSensor() const
{
  return temperatureSensor_;
}

const AlarmSettings &SettingsService::alarms() const
{
  return alarms_;
}

const SchoolBellSettings &SettingsService::schoolBell() const
{
  return schoolBell_;
}

AlarmMode SettingsService::alarmMode() const
{
  return alarmMode_;
}

String SettingsService::deviceName() const
{
  return deviceName_.isEmpty() ? DeviceIdentity::defaultName() : deviceName_;
}

const ContentSettings &SettingsService::content() const
{
  return content_;
}

bool SettingsService::usesDefaultApPassword() const
{
  return network_.apPassword == AppConfig::Network::apPassword;
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
      static_cast<uint8_t>(settings.connectionStyle)) == 1
    && preferences_.putUChar("panel_type", static_cast<uint8_t>(settings.type)) == 1
    && preferences_.putBool("hub12_data_inv", settings.hub12DataInverted) == 1
    && preferences_.putBool("hub12_oe_inv", settings.hub12OeInverted) == 1;
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
  stored = preferences_.putBool("sta_auth", settings.stationPortalAuthentication) == 1
    && stored;
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
    preferences_.putUChar("modbus_id", settings.slaveAddress) == 1
    && preferences_.putUInt("uart_baud", settings.baudRate) == 4
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

bool SettingsService::saveTimeSync(const TimeSyncSettings &settings)
{
  String error;
  if (!ready_ || !validateTimeSync(settings, error)) {
    return false;
  }

  bool stored = preferences_.putBool("ntp_enabled", settings.ntpEnabled) == 1;
  stored = storeString(preferences_, "ntp_server", settings.ntpServer) && stored;
  stored = storeString(preferences_, "ntp_tz", settings.timeZone) && stored;
  if (stored) {
    timeSync_ = settings;
  }
  return stored;
}

bool SettingsService::saveTemperatureSensor(const TemperatureSensorSettings &settings)
{
  String error;
  if (!ready_ || !validateTemperatureSensor(settings, error)) {
    return false;
  }

  const bool stored =
    preferences_.putUChar("ts_type", static_cast<uint8_t>(settings.type)) == 1
    && preferences_.putUShort("ts_interval", settings.measurementIntervalSeconds) == 2
    && preferences_.putShort("ts_t_offset", settings.temperatureOffsetTenths) == 2
    && preferences_.putShort("ts_h_offset", settings.humidityOffsetTenths) == 2
    && preferences_.putUChar("ts_sht_addr", settings.sht40Address) == 1
    && preferences_.putUChar("ts_sht_prec", static_cast<uint8_t>(settings.sht40Precision)) == 1
    && preferences_.putUChar("ts_tmp_addr", settings.tmp112Address) == 1
    && preferences_.putUChar("ts_tmp_rate", static_cast<uint8_t>(settings.tmp112ConversionRate)) == 1
    && preferences_.putBool("ts_tmp_ext", settings.tmp112ExtendedMode) == 1
    && preferences_.putUChar("ts_hdc_addr", settings.hdc2022Address) == 1
    && preferences_.putUChar("ts_hdc_tres", static_cast<uint8_t>(settings.hdc2022TemperatureResolution)) == 1
    && preferences_.putUChar("ts_hdc_hres", static_cast<uint8_t>(settings.hdc2022HumidityResolution)) == 1;
  if (stored) {
    temperatureSensor_ = settings;
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

bool SettingsService::saveSchoolBell(const SchoolBellSettings &settings)
{
  String error;
  if (!ready_ || !validateSchoolBell(settings, error)) {
    return false;
  }

  if (preferences_.putBytes("school_bell", &settings, sizeof(settings))
      != sizeof(settings)) {
    return false;
  }

  schoolBell_ = settings;
  return true;
}

bool SettingsService::saveAlarmMode(AlarmMode mode)
{
  if (!ready_ || static_cast<uint8_t>(mode) > static_cast<uint8_t>(AlarmMode::schoolBell)) {
    return false;
  }
  if (preferences_.putUChar("alarm_mode", static_cast<uint8_t>(mode)) != 1) {
    return false;
  }
  alarmMode_ = mode;
  return true;
}

bool SettingsService::saveDeviceName(const String &name)
{
  String error;
  if (!ready_ || !validateDeviceName(name, error)) {
    return false;
  }
  if (name.isEmpty()) {
    if (preferences_.isKey("dev_name") && !preferences_.remove("dev_name")) {
      return false;
    }
  } else if (preferences_.putString("dev_name", name) != name.length()) {
    return false;
  }
  deviceName_ = name;
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
  const bool timeSyncStored = saveTimeSync(timeSync_);
  const bool temperatureSensorStored = saveTemperatureSensor(temperatureSensor_);
  const bool alarmsStored = saveAlarms(alarms_);
  const bool schoolBellStored = saveSchoolBell(schoolBell_);
  const bool alarmModeStored = saveAlarmMode(alarmMode_);
  const bool contentStored = saveContent(content_);
  return panelStored
      && networkStored
      && communicationStored
      && sleepStored
      && timeSyncStored
      && temperatureSensorStored
      && alarmsStored
      && schoolBellStored
      && alarmModeStored
      && contentStored;
}

bool SettingsService::validatePanel(const PanelSettings &settings, String &error)
{
  if (settings.scanDivisor != 4 && settings.scanDivisor != 8) {
    error = UI_TEXT("Tarama oranı 1/4 veya 1/8 olmalıdır.");
    return false;
  }
  if (settings.rows < 1 || settings.rows > AppConfig::maxPanelRows) {
    error = UI_TEXT("Satır sayısı geçersiz.");
    return false;
  }
  if (settings.columns < 1 || settings.columns > AppConfig::maxPanelColumns) {
    error = UI_TEXT("Sütun sayısı geçersiz.");
    return false;
  }
  if (settings.panelCount() > AppConfig::maxPanelCount) {
    error = UI_TEXT("Toplam panel sayısı izin verilen sınırı aşıyor.");
    return false;
  }
  const uint8_t connectionStyle = static_cast<uint8_t>(settings.connectionStyle);
  if (connectionStyle < static_cast<uint8_t>(PanelConnectionStyle::topLeftDown)
      || connectionStyle > static_cast<uint8_t>(PanelConnectionStyle::bottomLeftUpZigzag)) {
    error = UI_TEXT("Panel bağlantı şekli geçersiz.");
    return false;
  }
  if (static_cast<uint8_t>(settings.type) > static_cast<uint8_t>(PanelType::monoHub12)) {
    error = UI_TEXT("Panel tipi geçersiz.");
    return false;
  }
  if (settings.type == PanelType::monoHub12 && settings.scanDivisor != 4) {
    error = UI_TEXT("Tek renk HUB12 paneller yalnızca 1/4 tarama ile desteklenir.");
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateNetwork(const NetworkSettings &settings, String &error)
{
  if (!isValidSsid(settings.apSsid, false)) {
    error = UI_TEXT("AP SSID 1-32 karakter olmalıdır.");
    return false;
  }
  if (!isValidPassword(settings.apPassword, false)) {
    error = UI_TEXT("AP parolası 8-63 karakter olmalıdır.");
    return false;
  }
  if (!isValidSsid(settings.staSsid, true)) {
    error = UI_TEXT("STA SSID en fazla 32 karakter olabilir.");
    return false;
  }
  if (!isValidPassword(settings.staPassword, true)) {
    error = UI_TEXT("STA parolası boş veya 8-63 karakter olmalıdır.");
    return false;
  }
  if (settings.staSsid.isEmpty() && !settings.staPassword.isEmpty()) {
    error = UI_TEXT("STA SSID boşken parola girilemez.");
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateCommunication(
  const CommunicationSettings &settings,
  String &error)
{
  if (settings.slaveAddress < 1 || settings.slaveAddress > 247) {
    error = UI_TEXT("Modbus Slave ID 1-247 aralığında olmalıdır.");
    return false;
  }
  const bool baudRateValid = settings.baudRate == 4800
    || settings.baudRate == 9600
    || settings.baudRate == 19200
    || settings.baudRate == 57600
    || settings.baudRate == 115200;
  if (!baudRateValid) {
    error = UI_TEXT("UART baudrate geçersiz.");
    return false;
  }
  if (settings.dataBits < 5 || settings.dataBits > 8) {
    error = UI_TEXT("UART data size 5-8 bit olmalıdır.");
    return false;
  }
  if (static_cast<uint8_t>(settings.parity)
      > static_cast<uint8_t>(UartParity::odd)) {
    error = UI_TEXT("UART parity ayarı geçersiz.");
    return false;
  }
  if (static_cast<uint8_t>(settings.handshake)
      > static_cast<uint8_t>(UartHandshake::rs485Direction)) {
    error = UI_TEXT("UART handshake ayarı geçersiz.");
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
    error = UI_TEXT("Uyku başlangıç veya bitiş saati geçersiz.");
    return false;
  }
  if (settings.startMinuteOfDay == settings.endMinuteOfDay) {
    error = UI_TEXT("Uyku başlangıç ve bitiş saati farklı olmalıdır.");
    return false;
  }
  if (static_cast<uint8_t>(settings.dayMode)
      > static_cast<uint8_t>(ScheduleDayMode::weekdays)) {
    error = UI_TEXT("Uyku açılış günü geçersiz.");
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateTimeSync(
  const TimeSyncSettings &settings,
  String &error)
{
  if (!isValidNtpServer(settings.ntpServer)) {
    error = UI_TEXT("NTP sunucusu 1-63 karakter olmalı; yalnızca harf, rakam, nokta, tire ve iki nokta içerebilir.");
    return false;
  }
  if (!isValidTimeZone(settings.timeZone)) {
    error = UI_TEXT("Saat dilimi 1-63 karakterlik geçerli bir POSIX TZ değeri olmalıdır.");
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::validateTemperatureSensor(
  const TemperatureSensorSettings &settings,
  String &error)
{
  if (static_cast<uint8_t>(settings.type)
      > static_cast<uint8_t>(TemperatureSensorType::hdc2022)) {
    error = UI_TEXT("Sıcaklık sensörü modeli geçersiz.");
    return false;
  }
  if (settings.measurementIntervalSeconds < minimumSensorIntervalSeconds
      || settings.measurementIntervalSeconds > maximumSensorIntervalSeconds) {
    error = UI_TEXT("Ölçüm aralığı 1-60 saniye olmalıdır.");
    return false;
  }
  if (settings.temperatureOffsetTenths < -maximumTemperatureOffsetTenths
      || settings.temperatureOffsetTenths > maximumTemperatureOffsetTenths) {
    error = UI_TEXT("Sıcaklık düzeltmesi -10.0 ile 10.0 °C arasında olmalıdır.");
    return false;
  }
  if (settings.humidityOffsetTenths < -maximumHumidityOffsetTenths
      || settings.humidityOffsetTenths > maximumHumidityOffsetTenths) {
    error = UI_TEXT("Nem düzeltmesi -20.0 ile 20.0 %RH arasında olmalıdır.");
    return false;
  }
  if ((settings.sht40Address != 0x44 && settings.sht40Address != 0x45)
      || static_cast<uint8_t>(settings.sht40Precision)
           > static_cast<uint8_t>(Sht40Precision::low)) {
    error = UI_TEXT("SHT40 ayarları geçersiz.");
    return false;
  }
  const bool tmp112AddressValid =
    (settings.tmp112Address >= 0x48 && settings.tmp112Address <= 0x4B)
    || (settings.tmp112Address >= 0x40 && settings.tmp112Address <= 0x43);
  if (!tmp112AddressValid
      || static_cast<uint8_t>(settings.tmp112ConversionRate)
           > static_cast<uint8_t>(Tmp112ConversionRate::hz8)) {
    error = UI_TEXT("TMP112 ayarları geçersiz.");
    return false;
  }
  if ((settings.hdc2022Address != 0x40 && settings.hdc2022Address != 0x41)
      || static_cast<uint8_t>(settings.hdc2022TemperatureResolution)
           > static_cast<uint8_t>(Hdc2022Resolution::bits9)
      || static_cast<uint8_t>(settings.hdc2022HumidityResolution)
           > static_cast<uint8_t>(Hdc2022Resolution::bits9)) {
    error = UI_TEXT("HDC2022 ayarları geçersiz.");
    return false;
  }
  error.clear();
  return true;
}

bool SettingsService::contentUsesHumidity(const ContentSettings &settings)
{
  for (uint8_t index = 0; index < settings.count && index < maxContentItems; ++index) {
    const ContentItemSettings &item = settings.items[index];
    if (item.boxKind != ContentBoxKind::textBox) {
      continue;
    }
    if (item.cycleEnabled
        ? (item.cycleContentMask & contentTypeMask(ContentType::humidity)) != 0
        : item.type == ContentType::humidity) {
      return true;
    }
  }
  return false;
}

bool SettingsService::validateAlarms(
  const AlarmSettings &settings,
  String &error)
{
  if (settings.schemaVersion != alarmSchemaVersion
      || settings.count > maximumAlarmCount) {
    error = UI_TEXT("Alarm veri formatı geçersiz.");
    return false;
  }

  for (uint8_t index = 0; index < settings.count; ++index) {
    const AlarmItemSettings &alarm = settings.items[index];
    if (alarm.hour > 23 || alarm.minute > 59) {
      error = UI_TEXT("Alarm saati geçersiz.");
      return false;
    }
    if (alarm.durationSeconds < minimumAlarmDurationSeconds
        || alarm.durationSeconds > maximumAlarmDurationSeconds) {
      error = UI_TEXT("Alarm tetiklenme süresi 1-3600 saniye olmalıdır.");
      return false;
    }
    if (static_cast<uint8_t>(alarm.dayMode)
        > static_cast<uint8_t>(ScheduleDayMode::customDays)) {
      error = UI_TEXT("Alarm gün seçimi geçersiz.");
      return false;
    }
    if ((alarm.customDayMask & ~alarmAllDaysMask) != 0
        || (alarm.dayMode == ScheduleDayMode::customDays
            && alarm.customDayMask == 0)) {
      error = UI_TEXT("Alarm için en az bir geçerli gün seçilmelidir.");
      return false;
    }
  }

  error.clear();
  return true;
}

bool SettingsService::validateDeviceName(const String &name, String &error)
{
  if (name.length() > AppConfig::maxDeviceNameBytes) {
    error = UI_TEXT("Cihaz adı en fazla 32 bayt olabilir.");
    return false;
  }
  for (size_t index = 0; index < name.length(); ++index) {
    const uint8_t character = static_cast<uint8_t>(name[index]);
    if (character < 0x20 || character == 0x7F) {
      error = UI_TEXT("Cihaz adı kontrol karakteri içeremez.");
      return false;
    }
  }
  error.clear();
  return true;
}

bool SettingsService::validateSchoolBell(
  const SchoolBellSettings &settings,
  String &error)
{
  if (settings.schemaVersion != schoolBellSchemaVersion
      || settings.lessonCount > maximumSchoolLessons
      || settings.vacationCount > maximumSchoolVacations) {
    error = UI_TEXT("Okul saati veri formatı geçersiz.");
    return false;
  }
  if (static_cast<uint8_t>(settings.dayMode)
      > static_cast<uint8_t>(ScheduleDayMode::customDays)) {
    error = UI_TEXT("Okul saati gün seçimi geçersiz.");
    return false;
  }
  if ((settings.customDayMask & ~alarmAllDaysMask) != 0
      || (settings.dayMode == ScheduleDayMode::customDays
          && settings.customDayMask == 0)) {
    error = UI_TEXT("Okul saati için en az bir gün seçilmelidir.");
    return false;
  }
  if (settings.durationSeconds < minimumAlarmDurationSeconds
      || settings.durationSeconds > maximumAlarmDurationSeconds) {
    error = UI_TEXT("Zil süresi 1-3600 saniye olmalıdır.");
    return false;
  }
  if (settings.studentBellLeadMinutes > maximumStudentBellLeadMinutes) {
    error = UI_TEXT("Öğrenci zili en fazla 10 dakika önce çalabilir.");
    return false;
  }

  const uint16_t lead = settings.studentBellLeadMinutes;
  uint16_t previousEnd = 0;
  for (uint8_t index = 0; index < settings.lessonCount; ++index) {
    const SchoolLesson &lesson = settings.lessons[index];
    const bool hasBreak = lesson.breakMinute != noSchoolBreak;
    if (lesson.startMinute >= minutesPerDay
        || (hasBreak && lesson.breakMinute >= minutesPerDay)) {
      error = UI_TEXT("Ders saati geçersiz.");
      return false;
    }
    if (hasBreak && lesson.breakMinute <= lesson.startMinute) {
      error = UI_TEXT("Teneffüs zili ders başlangıcından sonra olmalıdır.");
      return false;
    }
    if (index > 0 && lesson.startMinute <= previousEnd) {
      error = UI_TEXT("Dersler sırayla girilmelidir; her ders bir önceki dersin teneffüs zilinden sonra başlamalıdır.");
      return false;
    }
    if (lead > 0
        && (lesson.startMinute < lead
            || (index > 0 && lesson.startMinute - lead <= previousEnd))) {
      error = UI_TEXT("Öğrenci zili bir önceki dersin teneffüs zilinden sonra çalmalıdır; öğrenci zili süresini kısaltın.");
      return false;
    }
    previousEnd = hasBreak ? lesson.breakMinute : lesson.startMinute;
  }

  for (uint8_t index = 0; index < settings.vacationCount; ++index) {
    const SchoolVacation &vacation = settings.vacations[index];
    if (!isValidDateKey(vacation.firstDate) || !isValidDateKey(vacation.lastDate)) {
      error = UI_TEXT("Tatil dönemi tarihi geçersiz.");
      return false;
    }
    if (vacation.lastDate < vacation.firstDate) {
      error = UI_TEXT("Tatil döneminin bitişi başlangıcından önce olamaz.");
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
    error = UI_TEXT("İçerik formatı geçersiz.");
    return false;
  }

  uint8_t panelItemCounts[AppConfig::maxPanelCount] = {};
  const int16_t displayWidth = AppConfig::panelWidth * panel.columns;
  const int16_t displayHeight = AppConfig::panelHeight * panel.rows;

  for (uint8_t index = 0; index < settings.count; ++index) {
    const ContentItemSettings &item = settings.items[index];
    if (static_cast<uint8_t>(item.boxKind)
        > static_cast<uint8_t>(ContentBoxKind::valueBox)) {
      error = UI_TEXT("İçerik kutusu tipi geçersiz.");
      return false;
    }
    if (item.panelIndex >= panel.panelCount()) {
      error = UI_TEXT("İçerik kutusu panel numarası geçersiz.");
      return false;
    }
    if (++panelItemCounts[item.panelIndex] > maxBoxesPerPanel) {
      error = UI_TEXT("Bir panelde toplam en fazla iki TextBox/ValueBox olabilir.");
      return false;
    }
    if (item.x < 0 || item.x >= displayWidth
        || item.y < 0 || item.y >= displayHeight) {
      error = UI_TEXT("İçerik kutusu koordinatı ekran dışında.");
      return false;
    }

    const uint8_t expectedPanel =
      (item.y / AppConfig::panelHeight) * panel.columns
      + (item.x / AppConfig::panelWidth);
    if (expectedPanel != item.panelIndex) {
      error = UI_TEXT("İçerik kutusu koordinatı ile panel numarası uyuşmuyor.");
      return false;
    }

    if (static_cast<uint8_t>(item.rotation)
        > static_cast<uint8_t>(TextRotation::degrees270)) {
      error = UI_TEXT("İçerik kutusu döndürme açısı geçersiz.");
      return false;
    }
    if (item.fontId >= FontModel::fontCount) {
      error = UI_TEXT("İçerik kutusu yazı tipi geçersiz.");
      return false;
    }
    if (static_cast<uint8_t>(item.scrollArea)
        > static_cast<uint8_t>(ScrollArea::display)) {
      error = UI_TEXT("İçerik kutusu gösterim alanı geçersiz.");
      return false;
    }
    if (std::memchr(item.text, '\0', sizeof(item.text)) == nullptr) {
      error = UI_TEXT("İçerik kutusu metni sonlandırılmamış.");
      return false;
    }
    if (item.boxKind == ContentBoxKind::textBox) {
      if (static_cast<uint8_t>(item.temperatureDisplayFormat)
          > static_cast<uint8_t>(TemperatureDisplayFormat::integer)) {
        error = UI_TEXT("TextBox sıcaklık gösterim tipi geçersiz.");
        return false;
      }
      const uint8_t type = static_cast<uint8_t>(item.type);
      if (type > static_cast<uint8_t>(lastContentType)) {
        error = UI_TEXT("TextBox veri tipi geçersiz.");
        return false;
      }
      if (item.type == ContentType::countdown) {
        if (item.cycleEnabled) {
          error = UI_TEXT("Geri sayım içerik döngüsüne eklenemez.");
          return false;
        }
        if (item.countdownSeconds < 1 || item.countdownSeconds > maximumCountdownSeconds) {
          error = UI_TEXT("Geri sayım süresi 1 saniye ile 23:59:59 arasında olmalıdır.");
          return false;
        }
        if (item.countdownAlarmSeconds > maximumCountdownAlarmSeconds) {
          error = UI_TEXT("Geri sayım alarm süresi 0-3600 saniye olmalıdır.");
          return false;
        }
      }
      if (!item.cycleEnabled
          && item.type == ContentType::text
          && item.text[0] == '\0') {
        error = UI_TEXT("Metin TextBox'u boş olamaz.");
        return false;
      }
      if ((item.cycleContentMask & ~allCycleContentMask) != 0) {
        error = UI_TEXT("TextBox döngü içeriği geçersiz.");
        return false;
      }
      if (item.cycleEnabled) {
        if (item.cycleContentMask == 0) {
          error = UI_TEXT("Döngü için en az bir bilgi seçilmelidir.");
          return false;
        }
        if (item.cycleIntervalSeconds < minimumCycleIntervalSeconds
            || item.cycleIntervalSeconds > maximumCycleIntervalSeconds) {
          error = UI_TEXT("Döngü süresi 1-3600 saniye olmalıdır.");
          return false;
        }
        if ((item.cycleContentMask & contentTypeMask(ContentType::text)) != 0
            && item.text[0] == '\0') {
          error = UI_TEXT("Döngüdeki yazı içeriği boş olamaz.");
          return false;
        }
      }
    } else {
      if (static_cast<uint8_t>(item.valueSource)
          > static_cast<uint8_t>(ValueSource::digitalInput)) {
        error = UI_TEXT("ValueBox veri kaynağı geçersiz.");
        return false;
      }
      if (static_cast<uint8_t>(item.modbusValueType)
          > static_cast<uint8_t>(ModbusValueType::ieee754Float)) {
        error = UI_TEXT("ValueBox Modbus veri tipi geçersiz.");
        return false;
      }
      if (static_cast<uint8_t>(item.counterDirection)
          > static_cast<uint8_t>(CounterDirection::down)) {
        error = UI_TEXT("ValueBox sayaç yönü geçersiz.");
        return false;
      }
      if (!std::isfinite(item.initialFloatValue)) {
        error = UI_TEXT("ValueBox başlangıç float değeri geçersiz.");
        return false;
      }
      if (item.valueDecimals > maxValueDecimals) {
        error = UI_TEXT("ValueBox ondalık basamak sayısı 0-3 olmalıdır.");
        return false;
      }
      if (std::strlen(item.text) > maxValueSuffixLength) {
        error = UI_TEXT("ValueBox birimi en fazla 15 bayt olabilir.");
        return false;
      }
      if (item.valueSource == ValueSource::modbus) {
        if (item.modbusValueType == ModbusValueType::signedInteger16
            && (item.initialIntegerValue < INT16_MIN
                || item.initialIntegerValue > INT16_MAX)) {
          error = UI_TEXT("Modbus integer başlangıç değeri -32768 ile 32767 arasında olmalıdır.");
          return false;
        }
        if (item.modbusValueType == ModbusValueType::ieee754Float
            && item.registerAddress == UINT16_MAX) {
          error = UI_TEXT("IEEE-754 float için son register adresi kullanılamaz.");
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
            error = UI_TEXT("Modbus ValueBox register adresleri birbiriyle çakışmamalıdır.");
            return false;
          }
        }
      }
    }
    if (static_cast<uint8_t>(item.direction)
        > static_cast<uint8_t>(ScrollDirection::down)) {
      error = UI_TEXT("Kaydırma yönü geçersiz.");
      return false;
    }
    if (item.speedPixelsPerSecond < 1 || item.speedPixelsPerSecond > 100) {
      error = UI_TEXT("Kaydırma hızı 1-100 piksel/saniye olmalıdır.");
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
  timeSync_ = TimeSyncSettings{
    false,
    AppConfig::TimeSync::defaultNtpServer,
    AppConfig::TimeSync::defaultTimeZone
  };
  temperatureSensor_ = TemperatureSensorSettings{};
  initializeDefaultAlarms(alarms_);
  schoolBell_ = SchoolBellSettings{};
  alarmMode_ = AlarmMode::manual;
  deviceName_ = "";
  initializeDefaultContent(content_);
}
