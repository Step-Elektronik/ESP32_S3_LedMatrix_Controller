#include "PortalApplication.h"

#include <Arduino.h>
#include <WiFi.h>
#include <FontCatalog.h>
#include <cmath>
#include <limits.h>
#include <memory>
#include <new>

#include "AlarmApplication.h"
#include "AppConfig.h"
#include "BootGuardService.h"
#include "CalendarDate.h"
#include "ClockApplication.h"
#include "CountdownApplication.h"
#include "FirmwareUpdateService.h"
#include "OnlineUpdateService.h"
#include "PanelContentApplication.h"
#include "PanelDisplay.h"
#include "PanelSleepApplication.h"
#include "PortalCompressed.h"
#include "SchoolBellSchedule.h"
#include "SettingsService.h"
#include "TemperatureApplication.h"
#include "TimeSyncApplication.h"
#include "TurkishHolidays.h"
#include "UiText.h"
#include "ValueBoxApplication.h"
#include "WiFiApplication.h"

namespace {

constexpr uint32_t restartDelayMs = 1500;
constexpr uint32_t colorTestDurationMs = 750;
constexpr size_t maximumContentBodySize = 8192;
constexpr size_t fontPreviewHeaderSize = 14;
constexpr size_t fontPreviewGlyphSize = 8;
constexpr uint8_t maximumContentFields = 25;

void writeLittleEndian16(uint8_t *target, uint16_t value)
{
  target[0] = static_cast<uint8_t>(value & 0xFFU);
  target[1] = static_cast<uint8_t>((value >> 8U) & 0xFFU);
}

void writeLittleEndian32(uint8_t *target, uint32_t value)
{
  target[0] = static_cast<uint8_t>(value & 0xFFU);
  target[1] = static_cast<uint8_t>((value >> 8U) & 0xFFU);
  target[2] = static_cast<uint8_t>((value >> 16U) & 0xFFU);
  target[3] = static_cast<uint8_t>((value >> 24U) & 0xFFU);
}

bool parseUnsignedByte(
  const String &text,
  uint8_t minimum,
  uint8_t maximum,
  uint8_t &result)
{
  if (text.isEmpty()) {
    return false;
  }

  char *end = nullptr;
  const unsigned long value = strtoul(text.c_str(), &end, 10);
  if (end == text.c_str() || *end != '\0' || value < minimum || value > maximum) {
    return false;
  }

  result = static_cast<uint8_t>(value);
  return true;
}

bool parseUnsigned16(
  const String &text,
  uint16_t minimum,
  uint16_t maximum,
  uint16_t &result)
{
  if (text.isEmpty()) {
    return false;
  }

  char *end = nullptr;
  const unsigned long value = strtoul(text.c_str(), &end, 10);
  if (end == text.c_str() || *end != '\0' || value < minimum || value > maximum) {
    return false;
  }

  result = static_cast<uint16_t>(value);
  return true;
}

bool parseUnsigned32(
  const String &text,
  uint32_t minimum,
  uint32_t maximum,
  uint32_t &result)
{
  if (text.isEmpty()) {
    return false;
  }

  char *end = nullptr;
  const unsigned long value = strtoul(text.c_str(), &end, 10);
  if (end == text.c_str() || *end != '\0' || value < minimum || value > maximum) {
    return false;
  }

  result = static_cast<uint32_t>(value);
  return true;
}

bool parseSigned32(const String &text, int32_t &result)
{
  if (text.isEmpty()) {
    return false;
  }

  char *end = nullptr;
  const long long value = strtoll(text.c_str(), &end, 10);
  if (end == text.c_str()
      || *end != '\0'
      || value < INT32_MIN
      || value > INT32_MAX) {
    return false;
  }

  result = static_cast<int32_t>(value);
  return true;
}

bool parseFiniteFloat(const String &text, float &result)
{
  if (text.isEmpty()) {
    return false;
  }

  char *end = nullptr;
  const float value = strtof(text.c_str(), &end);
  if (end == text.c_str() || *end != '\0' || !std::isfinite(value)) {
    return false;
  }

  result = value;
  return true;
}

bool parseDateTime(
  const String &date,
  const String &time,
  ClockDateTime &result)
{
  unsigned year = 0;
  unsigned month = 0;
  unsigned day = 0;
  unsigned hour = 0;
  unsigned minute = 0;
  unsigned second = 0;

  if (sscanf(date.c_str(), "%u-%u-%u", &year, &month, &day) != 3) {
    return false;
  }

  const int parsedTime = sscanf(time.c_str(), "%u:%u:%u", &hour, &minute, &second);
  if (parsedTime < 2) {
    return false;
  }

  if (year < 2000 || year > 2099
      || month < 1 || month > 12
      || day < 1 || day > 31
      || hour > 23 || minute > 59 || second > 59) {
    return false;
  }

  const DateTime value(year, month, day, hour, minute, second);
  if (!value.isValid()) {
    return false;
  }

  result = {
    static_cast<uint16_t>(year),
    static_cast<uint8_t>(month),
    static_cast<uint8_t>(day),
    static_cast<uint8_t>(hour),
    static_cast<uint8_t>(minute),
    static_cast<uint8_t>(second)
  };
  return true;
}

bool parseMinuteOfDay(const String &text, uint16_t &result)
{
  unsigned hour = 0;
  unsigned minute = 0;
  char trailing = '\0';
  if (sscanf(text.c_str(), "%u:%u%c", &hour, &minute, &trailing) != 2
      || hour > 23
      || minute > 59) {
    return false;
  }

  result = static_cast<uint16_t>(hour * 60 + minute);
  return true;
}

// "YYYY-MM-DD" from a date input, stored as YYYYMMDD.
bool parseDateKey(const String &text, uint32_t &key)
{
  unsigned year = 0;
  unsigned month = 0;
  unsigned day = 0;
  char trailing = '\0';
  if (sscanf(text.c_str(), "%u-%u-%u%c", &year, &month, &day, &trailing) != 3
      || year > 9999
      || month > 12
      || day > 31
      || !isValidDate(
        static_cast<uint16_t>(year),
        static_cast<uint8_t>(month),
        static_cast<uint8_t>(day))) {
    return false;
  }
  key = year * 10000UL + month * 100UL + day;
  return true;
}

int8_t hexValue(char character)
{
  if (character >= '0' && character <= '9') {
    return character - '0';
  }
  if (character >= 'a' && character <= 'f') {
    return character - 'a' + 10;
  }
  if (character >= 'A' && character <= 'F') {
    return character - 'A' + 10;
  }
  return -1;
}

bool decodePercentText(const String &encoded, String &decoded)
{
  decoded.clear();
  decoded.reserve(encoded.length());
  for (size_t index = 0; index < encoded.length(); ++index) {
    const char character = encoded[index];
    if (character == '%' && index + 2 < encoded.length()) {
      const int8_t high = hexValue(encoded[index + 1]);
      const int8_t low = hexValue(encoded[index + 2]);
      if (high < 0 || low < 0) {
        return false;
      }
      decoded += static_cast<char>((high << 4) | low);
      index += 2;
    } else if (character == '+') {
      decoded += ' ';
    } else {
      decoded += character;
    }
  }
  return true;
}

uint8_t splitContentRecord(
  const String &record,
  String fields[maximumContentFields])
{
  uint8_t field = 0;
  int start = 0;
  while (field < maximumContentFields - 1) {
    const int separator = record.indexOf('|', start);
    if (separator < 0) {
      break;
    }
    fields[field++] = record.substring(start, separator);
    start = separator + 1;
  }
  fields[field++] = record.substring(start);
  return fields[field - 1].indexOf('|') < 0 ? field : 0;
}

bool parseColor(
  const String &text,
  uint8_t &red,
  uint8_t &green,
  uint8_t &blue)
{
  if (text.length() != 6) {
    return false;
  }
  for (size_t index = 0; index < text.length(); ++index) {
    if (hexValue(text[index]) < 0) {
      return false;
    }
  }

  red = static_cast<uint8_t>(strtoul(text.substring(0, 2).c_str(), nullptr, 16));
  green = static_cast<uint8_t>(strtoul(text.substring(2, 4).c_str(), nullptr, 16));
  blue = static_cast<uint8_t>(strtoul(text.substring(4, 6).c_str(), nullptr, 16));
  return true;
}

// One record per line, fields separated by '|':
//  0 panel, 1 type, 2 rotation, 3 x, 4 y, 5 RRGGBB, 6 scrolling, 7 direction,
//  8 speed, 9 text (percent-encoded; ValueBox: unit suffix), 10 cycle,
//  11 cycle mask, 12 cycle interval, 13 kind, 14 initial value, 15 source,
//  16 register, 17 value type, 18 counter direction, 19 temperature format,
//  20 font, 21 scroll/clip area, 22 ValueBox decimals, 23 countdown
//  seconds, 24 countdown alarm seconds.
// Shorter records from older portal versions are still accepted.
bool parseContentPayload(
  const String &payload,
  ContentSettings &content,
  String &error)
{
  int lineStart = 0;
  while (lineStart < static_cast<int>(payload.length())) {
    int lineEnd = payload.indexOf('\n', lineStart);
    if (lineEnd < 0) {
      lineEnd = payload.length();
    }

    String record = payload.substring(lineStart, lineEnd);
    record.trim();
    lineStart = lineEnd + 1;
    if (record.isEmpty()) {
      continue;
    }
    if (content.count >= maxContentItems) {
      error = UI_TEXT("Çok fazla TextBox/ValueBox gönderildi.");
      return false;
    }

    String fields[maximumContentFields];
    const uint8_t fieldCount = splitContentRecord(record, fields);
    if (fieldCount != 10
        && fieldCount != 13
        && fieldCount != 19
        && fieldCount != 20
        && fieldCount != 21
        && fieldCount != 23
        && fieldCount != maximumContentFields) {
      error = UI_TEXT("İçerik kutusu veri formatı geçersiz.");
      return false;
    }

    ContentItemSettings &item = content.items[content.count];
    uint8_t type = 0;
    uint8_t rotation = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t scrolling = 0;
    uint8_t direction = 0;
    uint8_t cycleEnabled = 0;
    uint8_t cycleContentMask = 0;
    uint8_t boxKind = 0;
    uint8_t valueSource = 0;
    uint8_t modbusValueType = 0;
    uint8_t counterDirection = 0;
    uint8_t temperatureDisplayFormat = 0;
    uint8_t fontId = FontModel::defaultFontId;
    uint8_t scrollArea = static_cast<uint8_t>(ScrollArea::display);
    uint8_t valueDecimals = 0;
    if (!parseUnsignedByte(fields[0], 0, AppConfig::maxPanelCount - 1, item.panelIndex)
        || !parseUnsignedByte(
          fields[1],
          0,
          static_cast<uint8_t>(lastContentType),
          type)
        || !parseUnsignedByte(fields[2], 0, 3, rotation)
        || !parseUnsignedByte(
          fields[3],
          0,
          AppConfig::panelWidth * AppConfig::maxPanelColumns - 1,
          x)
        || !parseUnsignedByte(
          fields[4],
          0,
          AppConfig::panelHeight * AppConfig::maxPanelRows - 1,
          y)
        || !parseColor(fields[5], item.red, item.green, item.blue)
        || !parseUnsignedByte(fields[6], 0, 1, scrolling)
        || !parseUnsignedByte(fields[7], 0, 3, direction)
        || !parseUnsignedByte(fields[8], 1, 100, item.speedPixelsPerSecond)) {
      error = UI_TEXT("İçerik kutusu alanlarından biri geçersiz.");
      return false;
    }

    if (fieldCount >= 13) {
      if (!parseUnsignedByte(fields[10], 0, 1, cycleEnabled)
          || !parseUnsignedByte(
            fields[11],
            0,
            allCycleContentMask,
            cycleContentMask)
          || !parseUnsigned16(
            fields[12],
            minimumCycleIntervalSeconds,
            maximumCycleIntervalSeconds,
            item.cycleIntervalSeconds)) {
        error = UI_TEXT("TextBox döngü alanlarından biri geçersiz.");
        return false;
      }
    } else {
      cycleContentMask = contentTypeMask(static_cast<ContentType>(type));
      item.cycleIntervalSeconds = defaultCycleIntervalSeconds;
    }

    if (fieldCount >= 19) {
      if (!parseUnsignedByte(
            fields[13],
            static_cast<uint8_t>(ContentBoxKind::textBox),
            static_cast<uint8_t>(ContentBoxKind::valueBox),
            boxKind)
          || !parseUnsignedByte(
            fields[15],
            static_cast<uint8_t>(ValueSource::modbus),
            static_cast<uint8_t>(ValueSource::digitalInput),
            valueSource)
          || !parseUnsigned16(fields[16], 0, UINT16_MAX, item.registerAddress)
          || !parseUnsignedByte(
            fields[17],
            static_cast<uint8_t>(ModbusValueType::signedInteger16),
            static_cast<uint8_t>(ModbusValueType::ieee754Float),
            modbusValueType)
          || !parseUnsignedByte(
            fields[18],
            static_cast<uint8_t>(CounterDirection::up),
            static_cast<uint8_t>(CounterDirection::down),
            counterDirection)) {
        error = UI_TEXT("ValueBox alanlarından biri geçersiz.");
        return false;
      }

      item.boxKind = static_cast<ContentBoxKind>(boxKind);
      item.valueSource = static_cast<ValueSource>(valueSource);
      item.modbusValueType = static_cast<ModbusValueType>(modbusValueType);
      item.counterDirection = static_cast<CounterDirection>(counterDirection);
      if (item.boxKind == ContentBoxKind::valueBox) {
        if (item.valueSource == ValueSource::modbus
            && item.modbusValueType == ModbusValueType::ieee754Float) {
          if (!parseFiniteFloat(fields[14], item.initialFloatValue)) {
            error = UI_TEXT("ValueBox başlangıç float değeri geçersiz.");
            return false;
          }
        } else if (!parseSigned32(fields[14], item.initialIntegerValue)) {
          error = UI_TEXT("ValueBox başlangıç integer değeri geçersiz.");
          return false;
        }
      }
    }

    if (fieldCount >= 20
        && !parseUnsignedByte(
          fields[19],
          static_cast<uint8_t>(TemperatureDisplayFormat::decimal),
          static_cast<uint8_t>(TemperatureDisplayFormat::integer),
          temperatureDisplayFormat)) {
      error = UI_TEXT("TextBox sıcaklık gösterim tipi geçersiz.");
      return false;
    }

    if (fieldCount >= 21
        && !parseUnsignedByte(
          fields[20],
          0,
          FontModel::fontCount - 1,
          fontId)) {
      error = UI_TEXT("İçerik kutusu yazı tipi geçersiz.");
      return false;
    }

    if (fieldCount >= 23) {
      if (!parseUnsignedByte(
            fields[21],
            static_cast<uint8_t>(ScrollArea::panel),
            static_cast<uint8_t>(ScrollArea::display),
            scrollArea)) {
        error = UI_TEXT("İçerik kutusu gösterim alanı geçersiz.");
        return false;
      }
      if (!parseUnsignedByte(fields[22], 0, maxValueDecimals, valueDecimals)) {
        error = UI_TEXT("ValueBox ondalık basamak sayısı 0-3 olmalıdır.");
        return false;
      }
    } else if (item.boxKind == ContentBoxKind::valueBox
               && item.valueSource == ValueSource::modbus
               && item.modbusValueType == ModbusValueType::ieee754Float) {
      valueDecimals = 1;
    }

    if (fieldCount >= maximumContentFields
        && (!parseUnsigned32(fields[23], 0, maximumCountdownSeconds, item.countdownSeconds)
            || !parseUnsigned16(fields[24], 0, maximumCountdownAlarmSeconds, item.countdownAlarmSeconds))) {
      error = UI_TEXT("Geri sayım alanlarından biri geçersiz.");
      return false;
    }

    String decodedText;
    if (!decodePercentText(fields[9], decodedText)
        || decodedText.length() >= maxContentTextLength) {
      error = UI_TEXT("İçerik kutusu metni çok uzun veya geçersiz.");
      return false;
    }

    item.type = static_cast<ContentType>(type);
    item.rotation = static_cast<TextRotation>(rotation);
    item.x = x;
    item.y = y;
    item.scrolling = scrolling != 0;
    item.direction = static_cast<ScrollDirection>(direction);
    item.cycleEnabled = cycleEnabled != 0;
    item.cycleContentMask = cycleContentMask;
    item.temperatureDisplayFormat =
      static_cast<TemperatureDisplayFormat>(temperatureDisplayFormat);
    item.fontId = fontId;
    item.scrollArea = static_cast<ScrollArea>(scrollArea);
    item.valueDecimals = valueDecimals;
    strlcpy(item.text, decodedText.c_str(), sizeof(item.text));
    if (item.boxKind == ContentBoxKind::valueBox) {
      // ValueBoxes keep the text field as their unit suffix.
      item.type = ContentType::text;
      item.cycleEnabled = false;
      item.cycleContentMask = contentTypeMask(ContentType::text);
    } else {
      item.valueDecimals = 0;
    }
    if (item.boxKind == ContentBoxKind::textBox
        && item.type == ContentType::countdown) {
      // A countdown is never part of a content cycle.
      item.cycleEnabled = false;
      item.cycleContentMask = contentTypeMask(ContentType::text);
    }
    ++content.count;
  }

  error.clear();
  return true;
}

const char *stationStateKey(StationConnectionState state)
{
  switch (state) {
    case StationConnectionState::disabled: return "disabled";
    case StationConnectionState::waiting: return "waiting";
    case StationConnectionState::paused: return "paused";
    case StationConnectionState::connecting: return "connecting";
    case StationConnectionState::connected: return "connected";
    case StationConnectionState::failed: return "failed";
  }
  return "waiting";
}

const char *stationFailureKey(StationFailureReason reason)
{
  switch (reason) {
    case StationFailureReason::none: return "none";
    case StationFailureReason::noSsid: return "no_ssid";
    case StationFailureReason::authentication: return "authentication";
    case StationFailureReason::connectionLost: return "connection_lost";
    case StationFailureReason::timeout: return "timeout";
  }
  return "none";
}

} // namespace

PortalApplication::PortalApplication(
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
  OnlineUpdateService &onlineUpdate)
  : settings_(settings),
    panel_(panel),
    clock_(clock),
    temperature_(temperature),
    content_(content),
    valueBoxes_(valueBoxes),
    countdown_(countdown),
    alarm_(alarm),
    panelSleep_(panelSleep),
    wifi_(wifi),
    timeSync_(timeSync),
    bootGuard_(bootGuard),
    firmwareUpdate_(firmwareUpdate),
    onlineUpdate_(onlineUpdate),
    webServer_(AppConfig::Network::httpPort)
{
}

void PortalApplication::begin()
{
  configureRoutes();
  webServer_.begin();
  Serial.println("HTTP portal ready.");
}

void PortalApplication::update()
{
  webServer_.handleClient();

  if (colorTestActive_
      && millis() - colorTestStartedAt_ >= colorTestDurationMs) {
    colorTestActive_ = false;
    content_.setRenderingPaused(false);
    content_.refresh();
    panelSleep_.setTemporaryWakeOverride(false);
  }

  if (restartPending_ && millis() - restartScheduledAt_ >= restartDelayMs) {
    ESP.restart();
  }
}

void PortalApplication::configureRoutes()
{
  onAuthorized("/", HTTP_GET, &PortalApplication::sendHomePage);
  onAuthorized("/settings", HTTP_GET, &PortalApplication::sendSettingsPage);
  onAuthorized("/i18n.js", HTTP_GET, &PortalApplication::sendI18nScript);
  onAuthorized("/firmware.js", HTTP_GET, &PortalApplication::sendFirmwareScript);

  onAuthorized("/api/settings", HTTP_GET, &PortalApplication::sendSettingsJson);
  onAuthorized("/api/wifi/status", HTTP_GET, &PortalApplication::sendWifiStatusJson);
  onAuthorized("/api/content", HTTP_GET, &PortalApplication::sendContentJson);
  onAuthorized("/api/content", HTTP_POST, &PortalApplication::saveContentSettings);
  onAuthorized("/api/font", HTTP_GET, &PortalApplication::sendFontPreviewData);
  onAuthorized("/api/temperature", HTTP_GET, &PortalApplication::sendTemperatureJson);
  onAuthorized("/api/countdown", HTTP_GET, &PortalApplication::sendCountdownJson);
  onAuthorized("/api/countdown", HTTP_POST, &PortalApplication::controlCountdown);
  onAuthorized("/api/settings/panel", HTTP_POST, &PortalApplication::savePanelSettings);
  onAuthorized("/api/settings/clock", HTTP_POST, &PortalApplication::saveClockSettings);
  onAuthorized("/api/settings/sleep", HTTP_POST, &PortalApplication::saveSleepSettings);
  onAuthorized("/api/settings/time-sync", HTTP_POST, &PortalApplication::saveTimeSyncSettings);
  onAuthorized("/api/settings/temperature-sensor", HTTP_POST, &PortalApplication::saveTemperatureSensorSettings);
  onAuthorized("/api/settings/alarms", HTTP_POST, &PortalApplication::saveAlarmSettings);
  onAuthorized("/api/alarm/test", HTTP_POST, &PortalApplication::testAlarmRelay);
  onAuthorized("/api/settings/communication", HTTP_POST, &PortalApplication::saveCommunicationSettings);
  onAuthorized("/api/settings/wifi", HTTP_POST, &PortalApplication::saveNetworkSettings);
  onAuthorized("/api/panel/test", HTTP_POST, &PortalApplication::startPanelColorTest);
  onAuthorized("/api/firmware/online", HTTP_GET, &PortalApplication::sendOnlineUpdateJson);
  onAuthorized("/api/firmware/online", HTTP_POST, &PortalApplication::controlOnlineUpdate);
  webServer_.on(
    "/api/firmware/update",
    HTTP_POST,
    [this]() {
      if (authorize()) {
        finishFirmwareUploadRequest();
      }
    },
    [this]() { handleFirmwareUpload(); });

  onAuthorized("/generate_204", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  onAuthorized("/gen_204", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  onAuthorized("/hotspot-detect.html", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  onAuthorized("/library/test/success.html", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  onAuthorized("/connecttest.txt", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  onAuthorized("/ncsi.txt", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  onAuthorized("/fwlink", HTTP_ANY, &PortalApplication::sendPortalRedirect);
  webServer_.onNotFound([this]() {
    if (authorize()) {
      sendPortalRedirect();
    }
  });
}

void PortalApplication::onAuthorized(
  const char *uri,
  HTTPMethod method,
  Handler handler)
{
  webServer_.on(uri, method, [this, handler]() {
    if (authorize()) {
      (this->*handler)();
    }
  });
}

bool PortalApplication::requestFromStation()
{
  return webServer_.client().localIP() != WiFi.softAPIP();
}

bool PortalApplication::authorized()
{
  // Joining the local AP already requires the AP password. When enabled in
  // the Wi-Fi settings, requests that arrive through the STA interface come
  // from a shared network and must present the same password with HTTP
  // Digest authentication.
  return !settings_.network().stationPortalAuthentication
      || !requestFromStation()
      || webServer_.authenticate(
        AppConfig::Network::portalUser,
        settings_.network().apPassword.c_str());
}

bool PortalApplication::authorize()
{
  if (authorized()) {
    return true;
  }
  webServer_.requestAuthentication(
    DIGEST_AUTH,
    AppConfig::Network::portalRealm,
    "Authentication required");
  return false;
}

void PortalApplication::sendHomePage()
{
  sendCompressedPage(
    PortalCompressed::homeGzip,
    PortalCompressed::homeGzipSize);
}

void PortalApplication::sendSettingsPage()
{
  sendCompressedPage(
    PortalCompressed::settingsGzip,
    PortalCompressed::settingsGzipSize);
}

void PortalApplication::sendI18nScript()
{
  sendCompressedPage(
    PortalCompressed::i18nGzip,
    PortalCompressed::i18nGzipSize,
    PSTR("application/javascript; charset=utf-8"));
}

void PortalApplication::sendFirmwareScript()
{
  String script;
  script.reserve(144);
  script += F(
    "document.querySelectorAll('[data-firmware-version]').forEach("
    "function(element){element.textContent=\"");
  script += escapeJson(AppConfig::firmwareVersion);
  script += F("\";});");

  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/javascript; charset=utf-8", script);
}

void PortalApplication::sendCompressedPage(
  const uint8_t *content,
  size_t contentLength,
  PGM_P contentType)
{
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.sendHeader("Cache-Control", "no-store, max-age=0");
  webServer_.sendHeader("Vary", "Accept-Encoding");
  webServer_.sendHeader("X-Content-Type-Options", "nosniff");
  webServer_.send_P(
    200,
    contentType,
    reinterpret_cast<PGM_P>(content),
    contentLength);
}

void PortalApplication::sendPortalRedirect()
{
  // Redirect to the address the client used, so the portal also opens from
  // the STA network instead of pointing at the unreachable AP address.
  IPAddress address = webServer_.client().localIP();
  if (address == IPAddress(0, 0, 0, 0)) {
    address = WiFi.softAPIP();
  }
  const String location = String("http://") + address.toString() + "/";
  webServer_.sendHeader("Location", location, true);
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(302, "text/plain; charset=utf-8", "Portal");
}

void PortalApplication::sendSettingsJson()
{
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildSettingsJson());
}

void PortalApplication::sendWifiStatusJson()
{
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildWifiStatusJson());
}

void PortalApplication::sendContentJson()
{
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildContentJson());
}

void PortalApplication::sendFontPreviewData()
{
  uint8_t fontId = 0;
  if (!webServer_.hasArg("id")
      || !parseUnsignedByte(
        webServer_.arg("id"),
        0,
        FontModel::fontCount - 1,
        fontId)) {
    sendJson(false, UI_TEXT("Yazı tipi kimliği geçersiz."), 400);
    return;
  }

  const uint8_t first = FontCatalog::firstGlyphCode;
  const uint8_t last = FontCatalog::lastGlyphCode;
  const uint8_t fontYAdvance = FontCatalog::yAdvance(fontId);
  const uint16_t glyphCount = static_cast<uint16_t>(last - first) + 1U;
  uint32_t bitmapSize = 0;
  uint8_t generatedBitmap[FontCatalog::generatedGlyphBufferSize] = {};
  // A glyph that cannot be produced is sent as an empty glyph, exactly as the
  // panel skips it, instead of failing the whole preview.
  for (uint16_t index = 0; index < glyphCount; ++index) {
    FontCatalog::GlyphView glyph;
    if (FontCatalog::glyph(
          fontId,
          static_cast<uint8_t>(first + index),
          generatedBitmap,
          sizeof(generatedBitmap),
          glyph)) {
      bitmapSize += glyph.bitmapSize;
    }
  }
  if (bitmapSize > UINT16_MAX) {
    sendJson(false, UI_TEXT("Yazı tipi önizleme verisi çok büyük."), 500);
    return;
  }
  const size_t responseSize = fontPreviewHeaderSize
    + static_cast<size_t>(glyphCount) * fontPreviewGlyphSize
    + bitmapSize;

  uint8_t header[fontPreviewHeaderSize] = {
    'G', 'F', 'X', '1', fontId, first, last, fontYAdvance, 0, 0, 0, 0, 0, 0
  };
  writeLittleEndian16(&header[8], glyphCount);
  writeLittleEndian32(&header[10], bitmapSize);

  webServer_.sendHeader("Cache-Control", "no-store, max-age=0");
  webServer_.sendHeader("X-Content-Type-Options", "nosniff");
  webServer_.setContentLength(responseSize);
  webServer_.send(200, "application/octet-stream", "");
  webServer_.sendContent(
    reinterpret_cast<const char *>(header),
    sizeof(header));

  uint8_t glyphChunk[fontPreviewGlyphSize * 16] = {};
  size_t glyphChunkLength = 0;
  uint16_t bitmapOffset = 0;
  for (uint16_t index = 0; index < glyphCount; ++index) {
    FontCatalog::GlyphView glyph;
    if (!FontCatalog::glyph(
          fontId,
          static_cast<uint8_t>(first + index),
          generatedBitmap,
          sizeof(generatedBitmap),
          glyph)) {
      glyph = {};
    }
    uint8_t *record = &glyphChunk[glyphChunkLength];
    writeLittleEndian16(record, bitmapOffset);
    record[2] = glyph.width;
    record[3] = glyph.height;
    record[4] = glyph.xAdvance;
    record[5] = static_cast<uint8_t>(glyph.xOffset);
    record[6] = static_cast<uint8_t>(glyph.yOffset);
    record[7] = 0;
    glyphChunkLength += fontPreviewGlyphSize;
    bitmapOffset += glyph.bitmapSize;

    if (glyphChunkLength == sizeof(glyphChunk)
        || index + 1U == glyphCount) {
      webServer_.sendContent(
        reinterpret_cast<const char *>(glyphChunk),
        glyphChunkLength);
      glyphChunkLength = 0;
    }
  }

  for (uint16_t index = 0; index < glyphCount; ++index) {
    FontCatalog::GlyphView glyph;
    if (!FontCatalog::glyph(
          fontId,
          static_cast<uint8_t>(first + index),
          generatedBitmap,
          sizeof(generatedBitmap),
          glyph)
        || glyph.bitmapSize == 0) {
      continue;
    }
    if (glyph.bitmapInProgramMemory) {
      webServer_.sendContent_P(
        reinterpret_cast<PGM_P>(glyph.bitmap),
        glyph.bitmapSize);
    } else {
      webServer_.sendContent(
        reinterpret_cast<const char *>(glyph.bitmap),
        glyph.bitmapSize);
    }
  }
}

void PortalApplication::sendTemperatureJson()
{
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildTemperatureJson());
}

void PortalApplication::sendCountdownJson()
{
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildCountdownJson());
}

void PortalApplication::controlCountdown()
{
  const String action = webServer_.arg("action");
  String message;
  if (action == "start") {
    if (!countdown_.start()) {
      sendJson(false, UI_TEXT("Tasarımda geri sayım kutusu yok. Önce bir TextBox'ın veri türünü Geri Sayım yapıp panele uygulayın."), 409);
      return;
    }
    message = UI_TEXT("Geri sayım başlatıldı.");
  } else if (action == "pause") {
    countdown_.pause();
    message = UI_TEXT("Geri sayım durduruldu.");
  } else if (action == "reset") {
    countdown_.reset();
    message = UI_TEXT("Geri sayım sıfırlandı.");
  } else {
    sendJson(false, UI_TEXT("Geçersiz geri sayım komutu."), 400);
    return;
  }

  content_.refresh();
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildCountdownJson(message));
}

void PortalApplication::handleFirmwareUpload()
{
  HTTPUpload &upload = webServer_.upload();
  if (upload.status == UPLOAD_FILE_START) {
    // Check credentials before the first byte reaches the OTA partition; the
    // request handler that answers 401 only runs after the whole upload.
    uploadAuthorized_ = authorized();
    if (!uploadAuthorized_) {
      Serial.println("OTA upload rejected: authentication required.");
    }
    // The online update owns the OTA partition while it downloads.
    uploadBlocked_ = uploadAuthorized_ && onlineUpdate_.busy();
    if (uploadBlocked_) {
      Serial.println("OTA upload rejected: online update in progress.");
    }
  }
  if (!uploadAuthorized_ || uploadBlocked_) {
    return;
  }

  switch (upload.status) {
    case UPLOAD_FILE_START:
      firmwareUpdate_.beginUpload(upload.filename);
      break;
    case UPLOAD_FILE_WRITE:
      firmwareUpdate_.writeChunk(upload.buf, upload.currentSize);
      break;
    case UPLOAD_FILE_END:
      firmwareUpdate_.finishUpload();
      break;
    case UPLOAD_FILE_ABORTED:
      firmwareUpdate_.abortUpload();
      break;
  }
}

void PortalApplication::finishFirmwareUploadRequest()
{
  if (uploadBlocked_) {
    uploadBlocked_ = false;
    sendJson(false, UI_TEXT("Çevrimiçi güncelleme sürerken dosya yüklenemez."), 409);
    return;
  }
  if (!firmwareUpdate_.uploadStarted()) {
    sendJson(false, UI_TEXT("firmware.bin dosyası gönderilmedi."), 400);
    return;
  }

  const bool success = firmwareUpdate_.uploadSucceeded();
  sendJson(
    success,
    firmwareUpdate_.message(),
    firmwareUpdate_.responseStatusCode());

  if (success) {
    scheduleRestart();
  }
}

void PortalApplication::sendOnlineUpdateJson()
{
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildOnlineUpdateJson());
}

void PortalApplication::controlOnlineUpdate()
{
  const String action = webServer_.arg("action");
  String message;
  bool accepted = true;
  if (action == "check") {
    accepted = onlineUpdate_.requestCheck(message);
  } else if (action == "install") {
    accepted = onlineUpdate_.requestInstall(message);
  } else if (action == "auto") {
    const bool enabled = webServer_.arg("enabled") == "1";
    onlineUpdate_.setAutomaticCheck(enabled);
    message = enabled
      ? UI_TEXT("Otomatik güncelleme denetimi açıldı.")
      : UI_TEXT("Otomatik güncelleme denetimi kapatıldı.");
  } else {
    sendJson(false, UI_TEXT("Geçersiz güncelleme komutu."), 400);
    return;
  }

  if (!accepted) {
    sendJson(false, message, 409);
    return;
  }
  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(200, "application/json; charset=utf-8", buildOnlineUpdateJson(message));
}

String PortalApplication::buildOnlineUpdateJson(const String &message) const
{
  static const char *const stateNames[] = {
    "idle", "checking", "upToDate", "available", "downloading", "installed", "failed"
  };
  const OnlineUpdateStatus status = onlineUpdate_.status();
  String json;
  json.reserve(512 + status.notes.length());
  json += F("{\"ok\":true,\"current\":\"");
  json += escapeJson(AppConfig::firmwareVersion);
  json += F("\",\"repository\":\"");
  json += escapeJson(AppConfig::OnlineUpdate::repositoryName);
  json += F("\",\"state\":\"");
  json += stateNames[static_cast<uint8_t>(status.state)];
  json += F("\",\"updateAvailable\":");
  json += status.updateAvailable ? F("true") : F("false");
  json += F(",\"automatic\":");
  json += status.automaticCheck ? F("true") : F("false");
  json += F(",\"stationConnected\":");
  json += status.stationConnected ? F("true") : F("false");
  json += F(",\"progress\":");
  json += status.progress;
  json += F(",\"secondsSinceCheck\":");
  json += status.secondsSinceCheck;
  json += F(",\"latest\":\"");
  json += escapeJson(status.latestVersion);
  json += F("\",\"notes\":\"");
  json += escapeJson(status.notes);
  json += F("\",\"size\":");
  json += status.size;
  json += F(",\"error\":\"");
  json += escapeJson(status.message);
  json += F("\",\"rolledBack\":\"");
  json += escapeJson(status.rolledBackVersion);
  json += F("\",\"message\":\"");
  json += escapeJson(message);
  json += F("\"}");
  return json;
}

void PortalApplication::saveContentSettings()
{
  const String payload = webServer_.arg("plain");
  if (payload.length() > maximumContentBodySize) {
    sendJson(false, UI_TEXT("Ekran içeriği veri sınırını aşıyor."), 413);
    return;
  }

  // ~2.7 KB; kept off the 8 KB loop task stack.
  std::unique_ptr<ContentSettings> content(new (std::nothrow) ContentSettings());
  if (!content) {
    sendJson(false, UI_TEXT("Ekran tasarımı için bellek ayrılamadı."), 500);
    return;
  }
  String error;
  if (!parseContentPayload(payload, *content, error)
      || !SettingsService::validateContent(*content, settings_.panel(), error)) {
    sendJson(false, error, 400);
    return;
  }
  if (!temperature_.humiditySupported()
      && SettingsService::contentUsesHumidity(*content)) {
    sendJson(false, UI_TEXT("Seçili sensör (TMP112) nem ölçmez; nem gösteren kutuları kaldırın veya veri türünü değiştirin."), 400);
    return;
  }
  if (!settings_.saveContent(*content)) {
    sendJson(false, UI_TEXT("Ekran tasarımı flash belleğe kaydedilemedi."), 500);
    return;
  }

  valueBoxes_.reload();
  countdown_.reload();
  content_.reload();
  sendJson(true, UI_TEXT("Ekran tasarımı panele uygulandı ve kaydedildi."));
}

void PortalApplication::savePanelSettings()
{
  String scan;
  String rows;
  String columns;
  String brightness;
  String connectionStyleText;
  PanelSettings panel{};
  uint8_t connectionStyle = 0;

  if (!postValue("scan", scan)
      || !postValue("rows", rows)
      || !postValue("columns", columns)
      || !postValue("brightness", brightness)
      || !postValue("connectionStyle", connectionStyleText)
      || !parseUnsignedByte(scan, 4, 8, panel.scanDivisor)
      || !parseUnsignedByte(rows, 1, AppConfig::maxPanelRows, panel.rows)
      || !parseUnsignedByte(columns, 1, AppConfig::maxPanelColumns, panel.columns)
      || !parseUnsignedByte(brightness, 0, 255, panel.brightness)
      || !parseUnsignedByte(connectionStyleText, 1, 8, connectionStyle)) {
    sendJson(false, UI_TEXT("Eksik veya geçersiz panel ayarı."), 400);
    return;
  }
  panel.connectionStyle = static_cast<PanelConnectionStyle>(connectionStyle);

  String error;
  if (!SettingsService::validatePanel(panel, error)) {
    sendJson(false, error, 400);
    return;
  }

  const PanelSettings current = settings_.panel();
  const bool layoutChanged = panel.scanDivisor != current.scanDivisor
    || panel.rows != current.rows
    || panel.columns != current.columns
    || panel.connectionStyle != current.connectionStyle;
  // Checked before saving: content is validated against the stored layout.
  const bool contentFits = !layoutChanged
    || SettingsService::validateContent(settings_.content(), panel, error);
  if (!settings_.savePanel(panel)) {
    sendJson(false, UI_TEXT("Panel ayarları flash belleğe kaydedilemedi."), 500);
    return;
  }

  if (!layoutChanged) {
    // Brightness is applied live; the sleep schedule and a running color
    // test pick up the stored value when they finish.
    if (!panelSleep_.sleeping() && !colorTestActive_) {
      panel_.setBrightness(panel.brightness);
    }
    sendJson(true, UI_TEXT("Parlaklık kaydedildi ve panele uygulandı."));
    return;
  }

  sendJson(
    true,
    contentFits
      ? UI_TEXT("Panel ayarları kaydedildi. Cihaz yeniden başlatılacak.")
      : UI_TEXT("Panel ayarları kaydedildi. Mevcut ekran tasarımı yeni yerleşime sığmadığı için yeniden başlatmada varsayılan tasarım yüklenecek."));
  scheduleRestart();
}

void PortalApplication::saveClockSettings()
{
  String date;
  String time;
  ClockDateTime dateTime{};
  if (!postValue("date", date)
      || !postValue("time", time)
      || !parseDateTime(date, time, dateTime)) {
    sendJson(false, UI_TEXT("Tarih veya saat geçersiz."), 400);
    return;
  }
  if (!clock_.ready()) {
    sendJson(false, UI_TEXT("RTC bulunamadı; saat ve tarih ayarlanamadı."), 503);
    return;
  }
  if (!clock_.setDateTime(dateTime)) {
    sendJson(false, UI_TEXT("Saat ve tarih RTC'ye yazılamadı."), 500);
    return;
  }

  content_.refresh();
  sendJson(true, UI_TEXT("Saat ve tarih kaydedildi."));
}

void PortalApplication::saveSleepSettings()
{
  String sleepEnabled;
  String sleepStart;
  String sleepEnd;
  String sleepDayModeText;
  PanelSleepSettings sleep{};
  uint8_t enabled = 0;
  uint8_t sleepDayMode = 0;
  if (!postValue("sleepEnabled", sleepEnabled)
      || !postValue("sleepStart", sleepStart)
      || !postValue("sleepEnd", sleepEnd)
      || !postValue("sleepDayMode", sleepDayModeText)
      || !parseUnsignedByte(sleepEnabled, 0, 1, enabled)
      || !parseMinuteOfDay(sleepStart, sleep.startMinuteOfDay)
      || !parseMinuteOfDay(sleepEnd, sleep.endMinuteOfDay)
      || !parseUnsignedByte(
        sleepDayModeText,
        static_cast<uint8_t>(ScheduleDayMode::everyDay),
        static_cast<uint8_t>(ScheduleDayMode::weekdays),
        sleepDayMode)) {
    sendJson(false, UI_TEXT("Uyku ayarı geçersiz."), 400);
    return;
  }
  sleep.enabled = enabled != 0;
  sleep.dayMode = static_cast<ScheduleDayMode>(sleepDayMode);

  String validationError;
  if (!SettingsService::validateSleep(sleep, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  if (!settings_.saveSleep(sleep)) {
    sendJson(false, UI_TEXT("Uyku ayarları flash belleğe kaydedilemedi."), 500);
    return;
  }

  sendJson(true, UI_TEXT("Panel uyku ayarları kaydedildi."));
}

void PortalApplication::saveTimeSyncSettings()
{
  String enabledText;
  TimeSyncSettings timeSync;
  uint8_t enabled = 0;
  if (!postValue("ntpEnabled", enabledText)
      || !postValue("ntpServer", timeSync.ntpServer)
      || !postValue("timeZone", timeSync.timeZone)
      || !parseUnsignedByte(enabledText, 0, 1, enabled)) {
    sendJson(false, UI_TEXT("Eksik veya geçersiz NTP ayarı."), 400);
    return;
  }
  timeSync.ntpEnabled = enabled != 0;
  timeSync.ntpServer.trim();
  timeSync.timeZone.trim();

  String validationError;
  if (!SettingsService::validateTimeSync(timeSync, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  if (!settings_.saveTimeSync(timeSync)) {
    sendJson(false, UI_TEXT("NTP ayarları flash belleğe kaydedilemedi."), 500);
    return;
  }

  timeSync_.reload();
  sendJson(true, UI_TEXT("NTP ayarları kaydedildi."));
}

void PortalApplication::saveTemperatureSensorSettings()
{
  String typeText;
  String intervalText;
  String temperatureOffsetText;
  String humidityOffsetText;
  String sht40AddressText;
  String sht40PrecisionText;
  String tmp112AddressText;
  String tmp112RateText;
  String tmp112ExtendedText;
  String hdc2022AddressText;
  String hdc2022TemperatureResolutionText;
  String hdc2022HumidityResolutionText;
  TemperatureSensorSettings sensor;
  uint8_t type = 0;
  int32_t temperatureOffset = 0;
  int32_t humidityOffset = 0;
  uint8_t sht40Precision = 0;
  uint8_t tmp112Rate = 0;
  uint8_t tmp112Extended = 0;
  uint8_t hdc2022TemperatureResolution = 0;
  uint8_t hdc2022HumidityResolution = 0;
  if (!postValue("type", typeText)
      || !postValue("interval", intervalText)
      || !postValue("temperatureOffset", temperatureOffsetText)
      || !postValue("humidityOffset", humidityOffsetText)
      || !postValue("sht40Address", sht40AddressText)
      || !postValue("sht40Precision", sht40PrecisionText)
      || !postValue("tmp112Address", tmp112AddressText)
      || !postValue("tmp112ConversionRate", tmp112RateText)
      || !postValue("tmp112ExtendedMode", tmp112ExtendedText)
      || !postValue("hdc2022Address", hdc2022AddressText)
      || !postValue("hdc2022TemperatureResolution", hdc2022TemperatureResolutionText)
      || !postValue("hdc2022HumidityResolution", hdc2022HumidityResolutionText)
      || !parseUnsignedByte(typeText, 0, 2, type)
      || !parseUnsigned16(
        intervalText,
        minimumSensorIntervalSeconds,
        maximumSensorIntervalSeconds,
        sensor.measurementIntervalSeconds)
      || !parseSigned32(temperatureOffsetText, temperatureOffset)
      || temperatureOffset < -maximumTemperatureOffsetTenths
      || temperatureOffset > maximumTemperatureOffsetTenths
      || !parseSigned32(humidityOffsetText, humidityOffset)
      || humidityOffset < -maximumHumidityOffsetTenths
      || humidityOffset > maximumHumidityOffsetTenths
      || !parseUnsignedByte(sht40AddressText, 0, 0x7F, sensor.sht40Address)
      || !parseUnsignedByte(sht40PrecisionText, 0, 2, sht40Precision)
      || !parseUnsignedByte(tmp112AddressText, 0, 0x7F, sensor.tmp112Address)
      || !parseUnsignedByte(tmp112RateText, 0, 3, tmp112Rate)
      || !parseUnsignedByte(tmp112ExtendedText, 0, 1, tmp112Extended)
      || !parseUnsignedByte(hdc2022AddressText, 0, 0x7F, sensor.hdc2022Address)
      || !parseUnsignedByte(hdc2022TemperatureResolutionText, 0, 2, hdc2022TemperatureResolution)
      || !parseUnsignedByte(hdc2022HumidityResolutionText, 0, 2, hdc2022HumidityResolution)) {
    sendJson(false, UI_TEXT("Eksik veya geçersiz sensör ayarı."), 400);
    return;
  }
  sensor.type = static_cast<TemperatureSensorType>(type);
  sensor.temperatureOffsetTenths = static_cast<int16_t>(temperatureOffset);
  sensor.humidityOffsetTenths = static_cast<int16_t>(humidityOffset);
  sensor.sht40Precision = static_cast<Sht40Precision>(sht40Precision);
  sensor.tmp112ConversionRate = static_cast<Tmp112ConversionRate>(tmp112Rate);
  sensor.tmp112ExtendedMode = tmp112Extended != 0;
  sensor.hdc2022TemperatureResolution =
    static_cast<Hdc2022Resolution>(hdc2022TemperatureResolution);
  sensor.hdc2022HumidityResolution =
    static_cast<Hdc2022Resolution>(hdc2022HumidityResolution);

  String validationError;
  if (!SettingsService::validateTemperatureSensor(sensor, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  // A design that shows humidity must not end up on a sensor without it.
  if (!temperatureSensorHasHumidity(sensor.type)
      && SettingsService::contentUsesHumidity(settings_.content())) {
    sendJson(false, UI_TEXT("Ekran tasarımında nem gösteren kutular var. TMP112 nem ölçmediği için önce bu kutuları kaldırın veya veri türünü değiştirin."), 409);
    return;
  }
  if (!settings_.saveTemperatureSensor(sensor)) {
    sendJson(false, UI_TEXT("Sensör ayarları flash belleğe kaydedilemedi."), 500);
    return;
  }

  temperature_.reload();
  content_.refresh();
  sendJson(true, UI_TEXT("Sıcaklık sensörü ayarları kaydedildi ve uygulandı."));
}

void PortalApplication::saveAlarmSettings()
{
  AlarmMode mode = settings_.alarmMode();
  String modeText;
  if (postValue("alarmMode", modeText)) {
    uint8_t value = 0;
    if (!parseUnsignedByte(
          modeText,
          static_cast<uint8_t>(AlarmMode::manual),
          static_cast<uint8_t>(AlarmMode::schoolBell),
          value)) {
      sendJson(false, UI_TEXT("Alarm modu geçersiz."), 400);
      return;
    }
    mode = static_cast<AlarmMode>(value);
  }

  AlarmSettings alarms{};
  alarms.schemaVersion = alarmSchemaVersion;

  String countText;
  if (!postValue("alarmCount", countText)
      || !parseUnsignedByte(
        countText,
        0,
        maximumAlarmCount,
        alarms.count)) {
    sendJson(false, UI_TEXT("Alarm sayısı geçersiz."), 400);
    return;
  }

  for (uint8_t index = 0; index < alarms.count; ++index) {
    char timeName[18];
    char durationName[22];
    char dayModeName[20];
    char dayMaskName[20];
    snprintf(timeName, sizeof(timeName), "alarmTime%u", index);
    snprintf(durationName, sizeof(durationName), "alarmDuration%u", index);
    snprintf(dayModeName, sizeof(dayModeName), "alarmDayMode%u", index);
    snprintf(dayMaskName, sizeof(dayMaskName), "alarmDayMask%u", index);

    String timeText;
    String durationText;
    String dayModeText;
    String dayMaskText;
    uint16_t minuteOfDay = 0;
    uint8_t dayMode = 0;
    uint8_t dayMask = alarmAllDaysMask;
    AlarmItemSettings &alarm = alarms.items[index];
    if (!postValue(timeName, timeText)
        || !postValue(durationName, durationText)
        || !postValue(dayModeName, dayModeText)
        || !parseMinuteOfDay(timeText, minuteOfDay)
        || !parseUnsigned16(
          durationText,
          minimumAlarmDurationSeconds,
          maximumAlarmDurationSeconds,
          alarm.durationSeconds)
        || !parseUnsignedByte(
          dayModeText,
          static_cast<uint8_t>(ScheduleDayMode::everyDay),
          static_cast<uint8_t>(ScheduleDayMode::customDays),
          dayMode)) {
      sendJson(false, UI_TEXT("Eksik veya geçersiz alarm ayarı."), 400);
      return;
    }

    const ScheduleDayMode parsedDayMode =
      static_cast<ScheduleDayMode>(dayMode);
    if (parsedDayMode == ScheduleDayMode::customDays) {
      if (!postValue(dayMaskName, dayMaskText)
          || !parseUnsignedByte(
            dayMaskText,
            1,
            alarmAllDaysMask,
            dayMask)) {
        sendJson(false, UI_TEXT("Alarm için en az bir geçerli gün seçilmelidir."), 400);
        return;
      }
    } else {
      dayMask = parsedDayMode == ScheduleDayMode::weekdays
        ? alarmWeekdaysMask
        : alarmAllDaysMask;
    }

    alarm.hour = minuteOfDay / 60;
    alarm.minute = minuteOfDay % 60;
    alarm.dayMode = parsedDayMode;
    alarm.customDayMask = dayMask;
  }

  // Older pages send no school bell fields; the stored schedule is kept.
  SchoolBellSettings school = settings_.schoolBell();
  String validationError;
  if (webServer_.hasArg("schoolLessonCount")
      && !parseSchoolBellSettings(school, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }

  if (!SettingsService::validateAlarms(alarms, validationError)
      || !SettingsService::validateSchoolBell(school, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  if (!settings_.saveAlarms(alarms)
      || !settings_.saveSchoolBell(school)
      || !settings_.saveAlarmMode(mode)) {
    sendJson(false, UI_TEXT("Alarm ayarları flash belleğe kaydedilemedi."), 500);
    return;
  }

  sendJson(
    true,
    mode == AlarmMode::schoolBell
      ? UI_TEXT("Alarm ayarları kaydedildi. Okul saati etkin.")
      : UI_TEXT("Alarm ayarları kaydedildi. Manuel alarm etkin."));
}

bool PortalApplication::parseSchoolBellSettings(
  SchoolBellSettings &school,
  String &error) const
{
  school = SchoolBellSettings{};
  String text;
  if (!postValue("schoolLessonCount", text)
      || !parseUnsignedByte(text, 0, maximumSchoolLessons, school.lessonCount)) {
    error = UI_TEXT("Ders sayısı geçersiz.");
    return false;
  }
  for (uint8_t index = 0; index < school.lessonCount; ++index) {
    char startName[20];
    char breakName[20];
    snprintf(startName, sizeof(startName), "lessonStart%u", index);
    snprintf(breakName, sizeof(breakName), "lessonBreak%u", index);
    SchoolLesson &lesson = school.lessons[index];
    lesson.breakMinute = noSchoolBreak;
    String breakText;
    if (!postValue(startName, text)
        || !parseMinuteOfDay(text, lesson.startMinute)
        || (postValue(breakName, breakText)
            && !breakText.isEmpty()
            && !parseMinuteOfDay(breakText, lesson.breakMinute))) {
      error = UI_TEXT("Eksik veya geçersiz ders saati.");
      return false;
    }
  }

  uint8_t dayMode = 0;
  if (!postValue("schoolDayMode", text)
      || !parseUnsignedByte(
        text,
        static_cast<uint8_t>(ScheduleDayMode::everyDay),
        static_cast<uint8_t>(ScheduleDayMode::customDays),
        dayMode)) {
    error = UI_TEXT("Okul saati gün seçimi geçersiz.");
    return false;
  }
  school.dayMode = static_cast<ScheduleDayMode>(dayMode);
  if (school.dayMode == ScheduleDayMode::customDays) {
    if (!postValue("schoolDayMask", text)
        || !parseUnsignedByte(text, 1, alarmAllDaysMask, school.customDayMask)) {
      error = UI_TEXT("Okul saati için en az bir gün seçilmelidir.");
      return false;
    }
  } else {
    school.customDayMask = school.dayMode == ScheduleDayMode::weekdays
      ? alarmWeekdaysMask
      : alarmAllDaysMask;
  }

  if (!postValue("schoolDuration", text)
      || !parseUnsigned16(
        text,
        minimumAlarmDurationSeconds,
        maximumAlarmDurationSeconds,
        school.durationSeconds)) {
    error = UI_TEXT("Zil süresi 1-3600 saniye olmalıdır.");
    return false;
  }
  if (!postValue("schoolStudentBell", text)
      || !parseUnsignedByte(
        text,
        0,
        maximumStudentBellLeadMinutes,
        school.studentBellLeadMinutes)) {
    error = UI_TEXT("Öğrenci zili en fazla 10 dakika önce çalabilir.");
    return false;
  }
  school.skipPublicHolidays = webServer_.arg("schoolSkipHolidays") == "1";
  school.remembranceBell = webServer_.arg("schoolRemembrance") == "1";

  if (!postValue("vacationCount", text)
      || !parseUnsignedByte(text, 0, maximumSchoolVacations, school.vacationCount)) {
    error = UI_TEXT("Tatil dönemi sayısı geçersiz.");
    return false;
  }
  for (uint8_t index = 0; index < school.vacationCount; ++index) {
    char startName[20];
    char endName[20];
    snprintf(startName, sizeof(startName), "vacationStart%u", index);
    snprintf(endName, sizeof(endName), "vacationEnd%u", index);
    String endText;
    SchoolVacation &vacation = school.vacations[index];
    if (!postValue(startName, text)
        || !postValue(endName, endText)
        || !parseDateKey(text, vacation.firstDate)
        || !parseDateKey(endText, vacation.lastDate)) {
      error = UI_TEXT("Tatil dönemi tarihi geçersiz.");
      return false;
    }
  }
  return true;
}

void PortalApplication::testAlarmRelay()
{
  String text;
  uint16_t seconds = 0;
  if (!postValue("seconds", text) || !parseUnsigned16(text, 1, 30, seconds)) {
    sendJson(false, UI_TEXT("Test süresi 1-30 saniye olmalıdır."), 400);
    return;
  }
  alarm_.testRelay(seconds);
  sendJson(true, UI_TEXT("Zil test için çalıştırıldı."));
}

void PortalApplication::saveNetworkSettings()
{
  NetworkSettings network;
  if (!postValue("apSsid", network.apSsid)
      || !postValue("apPassword", network.apPassword)
      || !postValue("staSsid", network.staSsid)
      || !postValue("staPassword", network.staPassword)) {
    sendJson(false, UI_TEXT("Eksik Wi-Fi ayarı."), 400);
    return;
  }

  // Stored passwords are never sent to the browser. An empty field with its
  // keep flag set means "leave the stored password unchanged".
  const NetworkSettings &current = settings_.network();
  String keep;
  if (postValue("apPasswordKeep", keep) && keep == "1") {
    network.apPassword = current.apPassword;
  }
  if (postValue("staPasswordKeep", keep) && keep == "1") {
    network.staPassword = current.staPassword;
  }
  String authentication;
  network.stationPortalAuthentication = postValue("staAuthentication", authentication)
    ? authentication == "1"
    : current.stationPortalAuthentication;

  String error;
  if (!SettingsService::validateNetwork(network, error)) {
    sendJson(false, error, 400);
    return;
  }
  if (!settings_.saveNetwork(network)) {
    sendJson(false, UI_TEXT("Wi-Fi ayarları flash belleğe kaydedilemedi."), 500);
    return;
  }

  sendJson(true, UI_TEXT("Wi-Fi ayarları kaydedildi. Cihaz yeniden başlatılacak."));
  scheduleRestart();
}

void PortalApplication::saveCommunicationSettings()
{
  String slaveAddressText;
  String baudRateText;
  String dataBitsText;
  String parityText;
  String handshakeText;
  CommunicationSettings communication{};
  uint8_t dataBits = 0;
  uint8_t slaveAddress = 0;
  uint8_t parity = 0;
  uint8_t handshake = 0;

  if (!postValue("slaveAddress", slaveAddressText)
      || !postValue("baudRate", baudRateText)
      || !postValue("dataBits", dataBitsText)
      || !postValue("parity", parityText)
      || !postValue("handshake", handshakeText)
      || !parseUnsignedByte(slaveAddressText, 1, 247, slaveAddress)
      || !parseUnsigned32(
        baudRateText,
        4800,
        115200,
        communication.baudRate)
      || !parseUnsignedByte(dataBitsText, 5, 8, dataBits)
      || !parseUnsignedByte(
        parityText,
        static_cast<uint8_t>(UartParity::none),
        static_cast<uint8_t>(UartParity::odd),
        parity)
      || !parseUnsignedByte(
        handshakeText,
        static_cast<uint8_t>(UartHandshake::none),
        static_cast<uint8_t>(UartHandshake::rs485Direction),
        handshake)) {
    sendJson(false, UI_TEXT("Eksik veya geçersiz haberleşme ayarı."), 400);
    return;
  }

  communication.slaveAddress = slaveAddress;
  communication.dataBits = dataBits;
  communication.parity = static_cast<UartParity>(parity);
  communication.handshake = static_cast<UartHandshake>(handshake);
  String validationError;
  if (!SettingsService::validateCommunication(
        communication,
        validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  if (!settings_.saveCommunication(communication)) {
    sendJson(
      false,
      UI_TEXT("Haberleşme ayarları flash belleğe kaydedilemedi."),
      500);
    return;
  }

  sendJson(
    true,
    UI_TEXT("Haberleşme ayarları kaydedildi. Cihaz yeniden başlatılacak."));
  scheduleRestart();
}

void PortalApplication::startPanelColorTest()
{
  if (!webServer_.hasArg("color")) {
    sendJson(false, UI_TEXT("Test rengi eksik."), 400);
    return;
  }
  if (!panel_.ready()) {
    sendJson(false, UI_TEXT("Panel kullanılabilir değil."), 503);
    return;
  }

  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  const String color = webServer_.arg("color");
  if (color == "red") {
    red = 255;
  } else if (color == "green") {
    green = 255;
  } else if (color == "blue") {
    blue = 255;
  } else {
    sendJson(false, UI_TEXT("Geçersiz test rengi."), 400);
    return;
  }

  const uint8_t testBrightness = min(
    settings_.panel().brightness,
    AppConfig::panelTestMaximumBrightness);
  panelSleep_.setTemporaryWakeOverride(true);
  content_.setRenderingPaused(true);
  panel_.setBrightness(testBrightness);
  panel_.showSolidColor(red, green, blue);
  colorTestActive_ = true;
  colorTestStartedAt_ = millis();
  sendJson(true, UI_TEXT("Renk testi başlatıldı."));
}

String PortalApplication::buildSettingsJson()
{
  const PanelSettings &panel = settings_.panel();
  const NetworkSettings &network = settings_.network();
  const CommunicationSettings &communication = settings_.communication();
  const PanelSleepSettings &sleep = settings_.sleep();
  const TimeSyncSettings &timeSync = settings_.timeSync();
  const AlarmSettings &alarms = settings_.alarms();
  const ClockDateTime clock = clock_.dateTime();

  // Sized for any uint8_t/uint16_t value so snprintf can never truncate.
  char date[16] = "";
  char time[16] = "";
  char sleepStart[12] = "";
  char sleepEnd[12] = "";
  if (clock_.ready()) {
    snprintf(date, sizeof(date), "%04u-%02u-%02u", clock.year, clock.month, clock.day);
    snprintf(time, sizeof(time), "%02u:%02u:%02u", clock.hour, clock.minute, clock.second);
  }

  snprintf(
    sleepStart,
    sizeof(sleepStart),
    "%02u:%02u",
    sleep.startMinuteOfDay / 60,
    sleep.startMinuteOfDay % 60);
  snprintf(
    sleepEnd,
    sizeof(sleepEnd),
    "%02u:%02u",
    sleep.endMinuteOfDay / 60,
    sleep.endMinuteOfDay % 60);

  String json;
  json.reserve(3584 + alarms.count * 80);
  json += F("{\"firmware\":\"");
  json += escapeJson(AppConfig::firmwareVersion);
  json += F("\",\"access\":\"");
  json += requestFromStation() ? F("sta") : F("ap");
  json += F("\",\"panel\":{\"scan\":");
  json += panel.scanDivisor;
  json += F(",\"rows\":");
  json += panel.rows;
  json += F(",\"columns\":");
  json += panel.columns;
  json += F(",\"count\":");
  json += panel.panelCount();
  json += F(",\"brightness\":");
  json += panel.brightness;
  json += F(",\"connectionStyle\":");
  json += static_cast<uint8_t>(panel.connectionStyle);
  json += F("},\"communication\":{\"baudRate\":");
  json += communication.baudRate;
  json += F(",\"slaveAddress\":");
  json += communication.slaveAddress;
  json += F(",\"dataBits\":");
  json += communication.dataBits;
  json += F(",\"parity\":");
  json += static_cast<uint8_t>(communication.parity);
  json += F(",\"handshake\":");
  json += static_cast<uint8_t>(communication.handshake);
  json += F("},\"clock\":{\"available\":");
  json += clock_.ready() ? F("true") : F("false");
  json += F(",\"date\":\"");
  json += date;
  json += F("\",\"time\":\"");
  json += time;
  json += F("\"},\"sleep\":{\"enabled\":");
  json += sleep.enabled ? F("true") : F("false");
  json += F(",\"start\":\"");
  json += sleepStart;
  json += F("\",\"end\":\"");
  json += sleepEnd;
  json += F("\",\"dayMode\":");
  json += static_cast<uint8_t>(sleep.dayMode);
  json += F("},\"timeSync\":{\"enabled\":");
  json += timeSync.ntpEnabled ? F("true") : F("false");
  json += F(",\"server\":\"");
  json += escapeJson(timeSync.ntpServer);
  json += F("\",\"timeZone\":\"");
  json += escapeJson(timeSync.timeZone);
  json += F("\",\"synchronized\":");
  json += timeSync_.synchronized() ? F("true") : F("false");
  json += F(",\"secondsSinceSync\":");
  json += timeSync_.secondsSinceSync();
  json += F(",\"stationConnected\":");
  json += wifi_.stationHasIpAddress() ? F("true") : F("false");
  const TemperatureSensorSettings &sensor = settings_.temperatureSensor();
  json += F("},\"temperatureSensor\":{\"type\":");
  json += static_cast<uint8_t>(sensor.type);
  json += F(",\"interval\":");
  json += sensor.measurementIntervalSeconds;
  json += F(",\"temperatureOffset\":");
  json += sensor.temperatureOffsetTenths;
  json += F(",\"humidityOffset\":");
  json += sensor.humidityOffsetTenths;
  json += F(",\"sht40Address\":");
  json += sensor.sht40Address;
  json += F(",\"sht40Precision\":");
  json += static_cast<uint8_t>(sensor.sht40Precision);
  json += F(",\"tmp112Address\":");
  json += sensor.tmp112Address;
  json += F(",\"tmp112ConversionRate\":");
  json += static_cast<uint8_t>(sensor.tmp112ConversionRate);
  json += F(",\"tmp112ExtendedMode\":");
  json += sensor.tmp112ExtendedMode ? F("true") : F("false");
  json += F(",\"hdc2022Address\":");
  json += sensor.hdc2022Address;
  json += F(",\"hdc2022TemperatureResolution\":");
  json += static_cast<uint8_t>(sensor.hdc2022TemperatureResolution);
  json += F(",\"hdc2022HumidityResolution\":");
  json += static_cast<uint8_t>(sensor.hdc2022HumidityResolution);
  json += F(",\"contentUsesHumidity\":");
  json += SettingsService::contentUsesHumidity(settings_.content()) ? F("true") : F("false");
  json += F("},\"alarms\":{\"maximum\":");
  json += maximumAlarmCount;
  json += F(",\"items\":[");
  for (uint8_t index = 0; index < alarms.count; ++index) {
    const AlarmItemSettings &alarm = alarms.items[index];
    if (index > 0) {
      json += ',';
    }
    json += F("{\"hour\":");
    json += alarm.hour;
    json += F(",\"minute\":");
    json += alarm.minute;
    json += F(",\"duration\":");
    json += alarm.durationSeconds;
    json += F(",\"dayMode\":");
    json += static_cast<uint8_t>(alarm.dayMode);
    json += F(",\"dayMask\":");
    json += alarm.customDayMask;
    json += '}';
  }
  json += F("]},\"alarmMode\":");
  json += static_cast<uint8_t>(settings_.alarmMode());
  json += F(",\"schoolBell\":");
  appendSchoolBellJson(json);
  // Passwords are write-only: the portal only learns whether one is set.
  json += F(",\"wifi\":{\"apSsid\":\"");
  json += escapeJson(network.apSsid);
  json += F("\",\"apPasswordDefault\":");
  json += settings_.usesDefaultApPassword() ? F("true") : F("false");
  json += F(",\"staSsid\":\"");
  json += escapeJson(network.staSsid);
  json += F("\",\"staPasswordSet\":");
  json += network.staPassword.isEmpty() ? F("false") : F("true");
  json += F(",\"staAuthentication\":");
  json += network.stationPortalAuthentication ? F("true") : F("false");
  json += F("}}");
  return json;
}

String PortalApplication::buildWifiStatusJson() const
{
  const NetworkSettings &network = settings_.network();
  const bool connected = wifi_.stationHasIpAddress();
  String json;
  json.reserve(240);
  json += F("{\"state\":\"");
  json += stationStateKey(wifi_.stationState());
  json += F("\",\"reason\":\"");
  json += stationFailureKey(wifi_.stationFailureReason());
  json += F("\",\"ssid\":\"");
  json += escapeJson(network.staSsid);
  json += F("\",\"ip\":\"");
  json += escapeJson(wifi_.stationIpAddress());
  json += F("\",\"retryInSeconds\":");
  json += wifi_.stationRetrySeconds();
  json += F(",\"rssi\":");
  if (connected) {
    json += wifi_.stationRssi();
  } else {
    json += F("null");
  }
  json += F(",\"quality\":");
  json += connected ? wifi_.stationSignalQuality() : 0;
  json += F("}");
  return json;
}

String PortalApplication::buildContentJson() const
{
  const PanelSettings &panel = settings_.panel();
  const ContentSettings &content = settings_.content();

  String json;
  json.reserve(1536 + content.count * 280);
  json += F("{\"ok\":true,\"panel\":{\"rows\":");
  json += panel.rows;
  json += F(",\"columns\":");
  json += panel.columns;
  json += F(",\"panelWidth\":");
  json += AppConfig::panelWidth;
  json += F(",\"panelHeight\":");
  json += AppConfig::panelHeight;
  json += F(",\"maxPerPanel\":");
  json += maxBoxesPerPanel;
  json += F("},");
  appendSensorJson(json);
  json += F(",\"defaultFont\":");
  json += FontModel::defaultFontId;
  json += F(",\"fonts\":[");
  for (uint8_t fontId = 0; fontId < FontModel::fontCount; ++fontId) {
    if (fontId > 0) {
      json += ',';
    }
    json += '\"';
    json += escapeJson(FontCatalog::name(fontId));
    json += '\"';
  }
  json += F("],\"items\":[");

  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    if (index > 0) {
      json += ',';
    }
    json += F("{\"panel\":");
    json += item.panelIndex;
    json += F(",\"kind\":");
    json += static_cast<uint8_t>(item.boxKind);
    json += F(",\"type\":");
    json += static_cast<uint8_t>(item.type);
    json += F(",\"rotation\":");
    json += static_cast<uint8_t>(item.rotation);
    json += F(",\"font\":");
    json += item.fontId;
    json += F(",\"x\":");
    json += item.x;
    json += F(",\"y\":");
    json += item.y;
    json += F(",\"red\":");
    json += item.red;
    json += F(",\"green\":");
    json += item.green;
    json += F(",\"blue\":");
    json += item.blue;
    json += F(",\"scrolling\":");
    json += item.scrolling ? F("true") : F("false");
    json += F(",\"direction\":");
    json += static_cast<uint8_t>(item.direction);
    json += F(",\"speed\":");
    json += item.speedPixelsPerSecond;
    json += F(",\"cycleEnabled\":");
    json += item.cycleEnabled ? F("true") : F("false");
    json += F(",\"cycleMask\":");
    json += item.cycleContentMask;
    json += F(",\"cycleInterval\":");
    json += item.cycleIntervalSeconds;
    json += F(",\"temperatureFormat\":");
    json += static_cast<uint8_t>(item.temperatureDisplayFormat);
    json += F(",\"scrollArea\":");
    json += static_cast<uint8_t>(item.scrollArea);
    json += F(",\"decimals\":");
    json += item.valueDecimals;
    json += F(",\"countdownSeconds\":");
    json += item.countdownSeconds;
    json += F(",\"countdownAlarm\":");
    json += item.countdownAlarmSeconds;
    json += F(",\"text\":\"");
    json += escapeJson(item.text);
    json += F("\",\"valueSource\":");
    json += static_cast<uint8_t>(item.valueSource);
    json += F(",\"registerAddress\":");
    json += item.registerAddress;
    json += F(",\"valueType\":");
    json += static_cast<uint8_t>(item.modbusValueType);
    json += F(",\"counterDirection\":");
    json += static_cast<uint8_t>(item.counterDirection);
    json += F(",\"initialValue\":");
    if (item.valueSource == ValueSource::modbus
        && item.modbusValueType == ModbusValueType::ieee754Float) {
      json += String(item.initialFloatValue, 6);
    } else {
      json += item.initialIntegerValue;
    }
    char currentValue[48] = {};
    valueBoxes_.formatValue(index, currentValue, sizeof(currentValue));
    json += F(",\"currentValue\":\"");
    json += escapeJson(currentValue);
    json += F("\",\"currentRaw\":");
    double currentNumber = 0.0;
    if (valueBoxes_.currentNumber(index, currentNumber)) {
      json += String(currentNumber, 6);
    } else {
      json += F("null");
    }
    json += '}';
  }
  json += F("]}");
  return json;
}

String PortalApplication::buildTemperatureJson() const
{
  String json;
  json.reserve(256);
  json += F("{\"ok\":true,");
  appendSensorJson(json);
  json += F(",\"found\":");
  json += temperature_.sensorFound() ? F("true") : F("false");
  json += F(",\"ready\":");
  json += temperature_.ready() ? F("true") : F("false");
  json += F(",\"temperature\":");
  if (temperature_.ready()) {
    json += String(temperature_.temperatureC(), 1);
  } else {
    json += F("null");
  }
  json += F(",\"humidity\":");
  if (temperature_.ready() && temperature_.humiditySupported()) {
    json += String(temperature_.relativeHumidity(), 1);
  } else {
    json += F("null");
  }
  json += '}';
  return json;
}

String PortalApplication::buildCountdownJson(const String &message) const
{
  static const char *const stateNames[] = {"idle", "running", "paused", "finished"};
  const ContentSettings &content = settings_.content();
  String json;
  json.reserve(160 + content.count * 96);
  json += F("{\"ok\":true,\"state\":\"");
  json += stateNames[static_cast<uint8_t>(countdown_.state())];
  json += F("\",\"elapsedMs\":");
  json += countdown_.elapsedMs();
  json += F(",\"alarmActive\":");
  json += countdown_.alarmActive() ? F("true") : F("false");
  json += F(",\"message\":\"");
  json += escapeJson(message);
  json += F("\",\"items\":[");
  bool first = true;
  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    if (!CountdownApplication::isCountdown(item)) {
      continue;
    }
    if (!first) {
      json += ',';
    }
    first = false;
    char text[16] = {};
    countdown_.format(item, text, sizeof(text));
    json += F("{\"index\":");
    json += index;
    json += F(",\"panel\":");
    json += item.panelIndex;
    json += F(",\"duration\":");
    json += item.countdownSeconds;
    json += F(",\"remaining\":");
    json += countdown_.remainingSeconds(item);
    json += F(",\"text\":\"");
    json += text;
    json += F("\"}");
  }
  json += F("]}");
  return json;
}

void PortalApplication::appendSchoolBellJson(String &json) const
{
  static const char *const stateNames[] = {
    "schoolDay", "notSchoolDay", "vacation", "publicHoliday", "halfDayHoliday"
  };
  static const char *const kindNames[] = {
    "lesson", "break", "studentCall", "remembrance"
  };
  const SchoolBellSettings &school = settings_.schoolBell();
  char text[16];
  auto appendTime = [&](uint16_t minute) {
    snprintf(text, sizeof(text), "%02u:%02u", minute / 60U, minute % 60U);
    json += text;
  };
  auto appendDate = [&](const CalendarDate &date) {
    snprintf(text, sizeof(text), "%04u-%02u-%02u", date.year, date.month, date.day);
    json += text;
  };
  auto dateFromKey = [](uint32_t key) {
    return CalendarDate{
      static_cast<uint16_t>(key / 10000UL),
      static_cast<uint8_t>((key / 100UL) % 100UL),
      static_cast<uint8_t>(key % 100UL)
    };
  };

  json += F("{\"maximumLessons\":");
  json += maximumSchoolLessons;
  json += F(",\"maximumVacations\":");
  json += maximumSchoolVacations;
  json += F(",\"dayMode\":");
  json += static_cast<uint8_t>(school.dayMode);
  json += F(",\"dayMask\":");
  json += school.customDayMask;
  json += F(",\"duration\":");
  json += school.durationSeconds;
  json += F(",\"studentBell\":");
  json += school.studentBellLeadMinutes;
  json += F(",\"skipHolidays\":");
  json += school.skipPublicHolidays ? F("true") : F("false");
  json += F(",\"remembrance\":");
  json += school.remembranceBell ? F("true") : F("false");
  json += F(",\"holidaysKnownUntil\":");
  json += TurkishHolidays::lastReligiousHolidayYear;
  json += F(",\"lessons\":[");
  for (uint8_t index = 0; index < school.lessonCount; ++index) {
    const SchoolLesson &lesson = school.lessons[index];
    if (index > 0) {
      json += ',';
    }
    json += F("{\"start\":\"");
    appendTime(lesson.startMinute);
    json += F("\",\"break\":\"");
    if (lesson.breakMinute != noSchoolBreak) {
      appendTime(lesson.breakMinute);
    }
    json += F("\"}");
  }
  json += F("],\"vacations\":[");
  for (uint8_t index = 0; index < school.vacationCount; ++index) {
    const SchoolVacation &vacation = school.vacations[index];
    if (index > 0) {
      json += ',';
    }
    json += F("{\"start\":\"");
    appendDate(dateFromKey(vacation.firstDate));
    json += F("\",\"end\":\"");
    appendDate(dateFromKey(vacation.lastDate));
    json += F("\"}");
  }

  json += F("],\"status\":");
  if (!clock_.ready()) {
    json += F("{\"clock\":false}}");
    return;
  }
  const ClockDateTime now = clock_.dateTime();
  const int32_t today = daysFromCivil(CalendarDate{now.year, now.month, now.day});
  const uint16_t minute = static_cast<uint16_t>(now.hour * 60U + now.minute);
  TurkishHolidayInfo holiday;
  const SchoolDayState state = SchoolBellSchedule::dayState(school, today, holiday);
  json += F("{\"clock\":true,\"state\":\"");
  json += stateNames[static_cast<uint8_t>(state)];
  json += F("\",\"today\":\"");
  appendDate(civilFromDays(today));
  json += F("\",\"holiday\":\"");
  json += escapeJson(TurkishHolidays::name(holiday.holiday));
  json += F("\",\"holidayHalf\":");
  json += holiday.halfDay ? F("true") : F("false");
  json += F(",\"next\":");
  int32_t bellDay = 0;
  SchoolBellEvent event;
  if (SchoolBellSchedule::nextBell(school, today, minute, bellDay, event)) {
    json += F("{\"date\":\"");
    appendDate(civilFromDays(bellDay));
    json += F("\",\"time\":\"");
    appendTime(event.minuteOfDay);
    json += F("\",\"kind\":\"");
    json += kindNames[static_cast<uint8_t>(event.kind)];
    json += F("\",\"lesson\":");
    json += event.lesson + 1;
    json += '}';
  } else {
    json += F("null");
  }

  // Public holiday days of the coming year so the schedule can be checked.
  json += F(",\"holidays\":[");
  uint8_t listed = 0;
  for (int32_t day = today; day < today + 400 && listed < 12; ++day) {
    const CalendarDate date = civilFromDays(day);
    const TurkishHolidayInfo info = TurkishHolidays::lookup(date);
    if (info.holiday == TurkishHoliday::none) {
      continue;
    }
    if (listed++ > 0) {
      json += ',';
    }
    json += F("{\"date\":\"");
    appendDate(date);
    json += F("\",\"name\":\"");
    json += escapeJson(TurkishHolidays::name(info.holiday));
    json += F("\",\"half\":");
    json += info.halfDay ? F("true") : F("false");
    json += '}';
  }
  json += F("]}}");
}

void PortalApplication::appendSensorJson(String &json) const
{
  json += F("\"sensor\":{\"type\":");
  json += static_cast<uint8_t>(temperature_.sensorType());
  json += F(",\"name\":\"");
  json += temperature_.sensorName();
  json += F("\",\"address\":");
  json += temperature_.sensorAddress();
  json += F(",\"humidity\":");
  json += temperature_.humiditySupported() ? F("true") : F("false");
  json += F(",\"missingText\":\"");
  json += temperature_.missingText();
  json += F("\"}");
}

void PortalApplication::sendJson(
  bool success,
  const String &message,
  int statusCode)
{
  String json = F("{\"ok\":");
  json += success ? F("true") : F("false");
  json += F(",\"message\":\"");
  json += escapeJson(message);
  json += F("\"}");

  webServer_.sendHeader("Cache-Control", "no-store");
  webServer_.send(statusCode, "application/json; charset=utf-8", json);
}

void PortalApplication::scheduleRestart()
{
  bootGuard_.prepareForPlannedRestart();
  restartPending_ = true;
  restartScheduledAt_ = millis();
}

bool PortalApplication::postValue(
  const char *name,
  String &value) const
{
  if (!webServer_.hasArg(name)) {
    return false;
  }
  value = webServer_.arg(name);
  return true;
}

String PortalApplication::escapeJson(const String &value)
{
  String escaped;
  escaped.reserve(value.length() + 8);
  for (size_t index = 0; index < value.length(); ++index) {
    const char character = value[index];
    if (character == '\\' || character == '"') {
      escaped += '\\';
      escaped += character;
    } else if (character == '\n') {
      escaped += F("\\n");
    } else if (character != '\r') {
      escaped += character;
    }
  }
  return escaped;
}
