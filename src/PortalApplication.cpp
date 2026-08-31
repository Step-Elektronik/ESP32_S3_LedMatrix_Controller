#include "PortalApplication.h"

#include <Arduino.h>
#include <WiFi.h>
#include <FontCatalog.h>
#include <cmath>
#include <limits.h>

#include "AppConfig.h"
#include "BootGuardService.h"
#include "ClockApplication.h"
#include "FirmwareUpdateService.h"
#include "PanelContentApplication.h"
#include "PanelDisplay.h"
#include "PanelSleepApplication.h"
#include "PortalCompressed.h"
#include "SettingsService.h"
#include "TemperatureApplication.h"
#include "ValueBoxApplication.h"
#include "WiFiApplication.h"

namespace {

constexpr uint32_t restartDelayMs = 1500;
constexpr uint32_t colorTestDurationMs = 750;
constexpr size_t maximumContentBodySize = 8192;
constexpr size_t fontPreviewHeaderSize = 14;
constexpr size_t fontPreviewGlyphSize = 8;

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

uint8_t splitContentRecord(const String &record, String fields[21])
{
  uint8_t field = 0;
  int start = 0;
  while (field < 20) {
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
      error = "Cok fazla TextBox gonderildi.";
      return false;
    }

    String fields[21];
    const uint8_t fieldCount = splitContentRecord(record, fields);
    if (fieldCount != 10
        && fieldCount != 13
        && fieldCount != 19
        && fieldCount != 20
        && fieldCount != 21) {
      error = "TextBox veri formati gecersiz.";
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
    if (!parseUnsignedByte(fields[0], 0, AppConfig::maxPanelCount - 1, item.panelIndex)
        || !parseUnsignedByte(fields[1], 0, 3, type)
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
      error = "TextBox alanlarindan biri gecersiz.";
      return false;
    }

    if (fieldCount == 13
        || fieldCount == 19
        || fieldCount == 20
        || fieldCount == 21) {
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
        error = "TextBox dongu alanlarindan biri gecersiz.";
        return false;
      }
    } else if (fieldCount == 10) {
      cycleContentMask = contentTypeMask(static_cast<ContentType>(type));
      item.cycleIntervalSeconds = defaultCycleIntervalSeconds;
    }

    if (fieldCount == 19 || fieldCount == 20 || fieldCount == 21) {
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
        error = "ValueBox alanlarindan biri gecersiz.";
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
            error = "ValueBox baslangic float degeri gecersiz.";
            return false;
          }
        } else if (!parseSigned32(fields[14], item.initialIntegerValue)) {
          error = "ValueBox baslangic integer degeri gecersiz.";
          return false;
        }
      }
    }

    if ((fieldCount == 20 || fieldCount == 21)
        && !parseUnsignedByte(
          fields[19],
          static_cast<uint8_t>(TemperatureDisplayFormat::decimal),
          static_cast<uint8_t>(TemperatureDisplayFormat::integer),
          temperatureDisplayFormat)) {
      error = "TextBox sicaklik gosterim tipi gecersiz.";
      return false;
    }

    if (fieldCount == 21
        && !parseUnsignedByte(
          fields[20],
          0,
          FontModel::fontCount - 1,
          fontId)) {
      error = "Icerik kutusu yazi tipi gecersiz.";
      return false;
    }

    String decodedText;
    if (!decodePercentText(fields[9], decodedText)
        || decodedText.length() >= maxContentTextLength) {
      error = "TextBox metni cok uzun veya gecersiz.";
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
    strlcpy(item.text, decodedText.c_str(), sizeof(item.text));
    if (item.boxKind == ContentBoxKind::valueBox) {
      item.type = ContentType::text;
      item.cycleEnabled = false;
      item.cycleContentMask = contentTypeMask(ContentType::text);
      item.text[0] = '\0';
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
  PanelSleepApplication &panelSleep,
  WiFiApplication &wifi,
  BootGuardService &bootGuard,
  FirmwareUpdateService &firmwareUpdate)
  : settings_(settings),
    panel_(panel),
    clock_(clock),
    temperature_(temperature),
    content_(content),
    valueBoxes_(valueBoxes),
    panelSleep_(panelSleep),
    wifi_(wifi),
    bootGuard_(bootGuard),
    firmwareUpdate_(firmwareUpdate),
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
  webServer_.on("/", HTTP_GET, [this]() {
    sendHomePage();
  });
  webServer_.on("/settings", HTTP_GET, [this]() {
    sendSettingsPage();
  });
  webServer_.on("/firmware.js", HTTP_GET, [this]() {
    sendFirmwareScript();
  });

  webServer_.on("/api/settings", HTTP_GET, [this]() {
    sendSettingsJson();
  });
  webServer_.on("/api/wifi/status", HTTP_GET, [this]() {
    sendWifiStatusJson();
  });
  webServer_.on("/api/content", HTTP_GET, [this]() {
    sendContentJson();
  });
  webServer_.on("/api/content", HTTP_POST, [this]() {
    saveContentSettings();
  });
  webServer_.on("/api/font", HTTP_GET, [this]() {
    sendFontPreviewData();
  });
  webServer_.on("/api/temperature", HTTP_GET, [this]() {
    sendTemperatureJson();
  });
  webServer_.on("/api/settings/panel", HTTP_POST, [this]() {
    savePanelSettings();
  });
  webServer_.on("/api/settings/clock", HTTP_POST, [this]() {
    saveClockSettings();
  });
  webServer_.on("/api/settings/alarms", HTTP_POST, [this]() {
    saveAlarmSettings();
  });
  webServer_.on("/api/settings/communication", HTTP_POST, [this]() {
    saveCommunicationSettings();
  });
  webServer_.on("/api/settings/wifi", HTTP_POST, [this]() {
    saveNetworkSettings();
  });
  webServer_.on("/api/panel/test", HTTP_POST, [this]() {
    startPanelColorTest();
  });
  webServer_.on(
    "/api/firmware/update",
    HTTP_POST,
    [this]() { finishFirmwareUploadRequest(); },
    [this]() { handleFirmwareUpload(); });

  const auto captiveRedirect = [this]() { sendPortalRedirect(); };
  webServer_.on("/generate_204", HTTP_ANY, captiveRedirect);
  webServer_.on("/gen_204", HTTP_ANY, captiveRedirect);
  webServer_.on("/hotspot-detect.html", HTTP_ANY, captiveRedirect);
  webServer_.on("/library/test/success.html", HTTP_ANY, captiveRedirect);
  webServer_.on("/connecttest.txt", HTTP_ANY, captiveRedirect);
  webServer_.on("/ncsi.txt", HTTP_ANY, captiveRedirect);
  webServer_.on("/fwlink", HTTP_ANY, captiveRedirect);
  webServer_.onNotFound(captiveRedirect);
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
  size_t contentLength)
{
  webServer_.sendHeader("Content-Encoding", "gzip");
  webServer_.sendHeader("Cache-Control", "no-store, max-age=0");
  webServer_.sendHeader("Vary", "Accept-Encoding");
  webServer_.sendHeader("X-Content-Type-Options", "nosniff");
  webServer_.send_P(
    200,
    PSTR("text/html; charset=utf-8"),
    reinterpret_cast<PGM_P>(content),
    contentLength);
}

void PortalApplication::sendPortalRedirect()
{
  const String location = String("http://") + WiFi.softAPIP().toString() + "/";
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
    sendJson(false, F("Yazi tipi kimligi gecersiz."), 400);
    return;
  }

  const uint8_t first = FontCatalog::firstGlyphCode;
  const uint8_t last = FontCatalog::lastGlyphCode;
  const uint8_t fontYAdvance = FontCatalog::yAdvance(fontId);
  const uint16_t glyphCount = static_cast<uint16_t>(last - first) + 1U;
  uint32_t bitmapSize = 0;
  uint8_t generatedBitmap[FontCatalog::generatedGlyphBufferSize] = {};
  for (uint16_t index = 0; index < glyphCount; ++index) {
    FontCatalog::GlyphView glyph;
    if (!FontCatalog::glyph(
          fontId,
          static_cast<uint8_t>(first + index),
          generatedBitmap,
          sizeof(generatedBitmap),
          glyph)) {
      sendJson(false, F("Yazi tipi glif verisi olusturulamadi."), 500);
      return;
    }
    bitmapSize += glyph.bitmapSize;
  }
  if (bitmapSize > UINT16_MAX) {
    sendJson(false, F("Yazi tipi onizleme verisi cok buyuk."), 500);
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
    FontCatalog::glyph(
      fontId,
      static_cast<uint8_t>(first + index),
      generatedBitmap,
      sizeof(generatedBitmap),
      glyph);
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
    FontCatalog::glyph(
      fontId,
      static_cast<uint8_t>(first + index),
      generatedBitmap,
      sizeof(generatedBitmap),
      glyph);
    if (glyph.bitmapSize == 0) {
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

void PortalApplication::handleFirmwareUpload()
{
  HTTPUpload &upload = webServer_.upload();
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
  if (!firmwareUpdate_.uploadStarted()) {
    sendJson(false, F("firmware.bin dosyası gönderilmedi."), 400);
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

void PortalApplication::saveContentSettings()
{
  const String payload = webServer_.arg("plain");
  if (payload.length() > maximumContentBodySize) {
    sendJson(false, "Ekran icerigi veri sinirini asiyor.", 413);
    return;
  }

  ContentSettings content{};
  String error;
  if (!parseContentPayload(payload, content, error)
      || !SettingsService::validateContent(content, settings_.panel(), error)) {
    sendJson(false, error, 400);
    return;
  }
  if (!settings_.saveContent(content)) {
    sendJson(false, "Ekran tasarimi flash bellege kaydedilemedi.", 500);
    return;
  }

  valueBoxes_.reload();
  content_.reload();
  sendJson(true, "Ekran tasarimi panele uygulandi ve kaydedildi.");
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
    sendJson(false, "Eksik veya gecersiz panel ayari.", 400);
    return;
  }
  panel.connectionStyle = static_cast<PanelConnectionStyle>(connectionStyle);

  String error;
  if (!SettingsService::validatePanel(panel, error)) {
    sendJson(false, error, 400);
    return;
  }
  if (!settings_.savePanel(panel)) {
    sendJson(false, "Panel ayarlari flash bellege kaydedilemedi.", 500);
    return;
  }

  sendJson(true, "Panel ayarlari kaydedildi. Cihaz yeniden baslatilacak.");
  scheduleRestart();
}

void PortalApplication::saveClockSettings()
{
  String date;
  String time;
  String sleepEnabled;
  String sleepStart;
  String sleepEnd;
  String sleepDayModeText;
  ClockDateTime dateTime{};
  PanelSleepSettings sleep{};
  uint8_t enabled = 0;
  uint8_t sleepDayMode = 0;
  if (!postValue("date", date)
      || !postValue("time", time)
      || !postValue("sleepEnabled", sleepEnabled)
      || !postValue("sleepStart", sleepStart)
      || !postValue("sleepEnd", sleepEnd)
      || !postValue("sleepDayMode", sleepDayModeText)
      || !parseDateTime(date, time, dateTime)
      || !parseUnsignedByte(sleepEnabled, 0, 1, enabled)
      || !parseMinuteOfDay(sleepStart, sleep.startMinuteOfDay)
      || !parseMinuteOfDay(sleepEnd, sleep.endMinuteOfDay)
      || !parseUnsignedByte(
        sleepDayModeText,
        static_cast<uint8_t>(ScheduleDayMode::everyDay),
        static_cast<uint8_t>(ScheduleDayMode::weekdays),
        sleepDayMode)) {
    sendJson(false, "Tarih, saat veya uyku ayari gecersiz.", 400);
    return;
  }
  sleep.enabled = enabled != 0;
  sleep.dayMode = static_cast<ScheduleDayMode>(sleepDayMode);

  String validationError;
  if (!SettingsService::validateSleep(sleep, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  if (!clock_.setDateTime(dateTime)) {
    sendJson(false, "Saat ve tarih RTC'ye yazilamadi.", 500);
    return;
  }
  if (!settings_.saveSleep(sleep)) {
    sendJson(false, "Uyku ayarlari flash bellege kaydedilemedi.", 500);
    return;
  }

  content_.refresh();
  sendJson(true, "Saat, tarih ve panel uyku ayarlari kaydedildi.");
}

void PortalApplication::saveAlarmSettings()
{
  AlarmSettings alarms{};
  alarms.schemaVersion = alarmSchemaVersion;

  String countText;
  if (!postValue("alarmCount", countText)
      || !parseUnsignedByte(
        countText,
        0,
        maximumAlarmCount,
        alarms.count)) {
    sendJson(false, "Alarm sayisi gecersiz.", 400);
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
      sendJson(false, "Eksik veya gecersiz alarm ayari.", 400);
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
        sendJson(false, "Alarm icin en az bir gecerli gun secilmelidir.", 400);
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

  String validationError;
  if (!SettingsService::validateAlarms(alarms, validationError)) {
    sendJson(false, validationError, 400);
    return;
  }
  if (!settings_.saveAlarms(alarms)) {
    sendJson(false, "Alarm ayarlari flash bellege kaydedilemedi.", 500);
    return;
  }

  sendJson(true, "Alarm ayarlari kaydedildi.");
}

void PortalApplication::saveNetworkSettings()
{
  NetworkSettings network;
  if (!postValue("apSsid", network.apSsid)
      || !postValue("apPassword", network.apPassword)
      || !postValue("staSsid", network.staSsid)
      || !postValue("staPassword", network.staPassword)) {
    sendJson(false, "Eksik Wi-Fi ayari.", 400);
    return;
  }

  String error;
  if (!SettingsService::validateNetwork(network, error)) {
    sendJson(false, error, 400);
    return;
  }
  if (!settings_.saveNetwork(network)) {
    sendJson(false, "Wi-Fi ayarlari flash bellege kaydedilemedi.", 500);
    return;
  }

  sendJson(true, "Wi-Fi ayarlari kaydedildi. Cihaz yeniden baslatilacak.");
  scheduleRestart();
}

void PortalApplication::saveCommunicationSettings()
{
  String baudRateText;
  String dataBitsText;
  String parityText;
  String handshakeText;
  CommunicationSettings communication{};
  uint8_t dataBits = 0;
  uint8_t parity = 0;
  uint8_t handshake = 0;

  if (!postValue("baudRate", baudRateText)
      || !postValue("dataBits", dataBitsText)
      || !postValue("parity", parityText)
      || !postValue("handshake", handshakeText)
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
    sendJson(false, "Eksik veya gecersiz haberlesme ayari.", 400);
    return;
  }

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
      "Haberlesme ayarlari flash bellege kaydedilemedi.",
      500);
    return;
  }

  sendJson(
    true,
    "Haberlesme ayarlari kaydedildi. Cihaz yeniden baslatilacak.");
  scheduleRestart();
}

void PortalApplication::startPanelColorTest()
{
  if (!webServer_.hasArg("color")) {
    sendJson(false, "Test rengi eksik.", 400);
    return;
  }
  if (!panel_.ready()) {
    sendJson(false, "Panel kullanilabilir degil.", 503);
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
    sendJson(false, "Gecersiz test rengi.", 400);
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
  sendJson(true, "Renk testi baslatildi.");
}

String PortalApplication::buildSettingsJson() const
{
  const PanelSettings panel = settings_.panel();
  const NetworkSettings network = settings_.network();
  const CommunicationSettings communication = settings_.communication();
  const PanelSleepSettings sleep = settings_.sleep();
  const AlarmSettings alarms = settings_.alarms();
  const ClockDateTime clock = clock_.dateTime();

  char date[11] = "";
  char time[9] = "";
  char sleepStart[6] = "";
  char sleepEnd[6] = "";
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
  json.reserve(768 + alarms.count * 80);
  json += F("{\"firmware\":\"");
  json += escapeJson(AppConfig::firmwareVersion);
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
  json += F("\",\"sleepEnabled\":");
  json += sleep.enabled ? F("true") : F("false");
  json += F(",\"sleepStart\":\"");
  json += sleepStart;
  json += F("\",\"sleepEnd\":\"");
  json += sleepEnd;
  json += F("\",\"sleepDayMode\":");
  json += static_cast<uint8_t>(sleep.dayMode);
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
  json += F("]},\"wifi\":{\"apSsid\":\"");
  json += escapeJson(network.apSsid);
  json += F("\",\"apPassword\":\"");
  json += escapeJson(network.apPassword);
  json += F("\",\"staSsid\":\"");
  json += escapeJson(network.staSsid);
  json += F("\",\"staPassword\":\"");
  json += escapeJson(network.staPassword);
  json += F("\"}}");
  return json;
}

String PortalApplication::buildWifiStatusJson() const
{
  const NetworkSettings network = settings_.network();
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
  const PanelSettings panel = settings_.panel();
  const ContentSettings content = settings_.content();

  String json;
  json.reserve(1536 + content.count * 240);
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
  json += F("},\"defaultFont\":");
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
    char currentValue[32] = {};
    valueBoxes_.formatValue(index, currentValue, sizeof(currentValue));
    json += F(",\"currentValue\":\"");
    json += escapeJson(currentValue);
    json += F("\"}");
  }
  json += F("]}");
  return json;
}

String PortalApplication::buildTemperatureJson() const
{
  String json;
  json.reserve(96);
  json += F("{\"ok\":true,\"found\":");
  json += temperature_.sensorFound() ? F("true") : F("false");
  json += F(",\"ready\":");
  json += temperature_.ready() ? F("true") : F("false");
  json += F(",\"temperature\":");
  if (temperature_.ready()) {
    json += String(temperature_.temperatureC(), 1);
  } else {
    json += F("null");
  }
  json += '}';
  return json;
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
