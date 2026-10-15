#pragma once

#include <cstddef>
#include <stdint.h>

#include "AppConfig.h"
#include "FontModel.h"

constexpr uint16_t contentSchemaVersion = 7;
constexpr uint8_t maxBoxesPerPanel = 2;
constexpr uint8_t maxTextBoxesPerPanel = maxBoxesPerPanel;
constexpr uint8_t maxContentItems = AppConfig::maxPanelCount * maxBoxesPerPanel;
constexpr size_t maxContentTextLength = 48;
// ValueBox reuses the text field as a unit suffix such as "°C" or " bar".
constexpr size_t maxValueSuffixLength = 15;
constexpr uint8_t maxValueDecimals = 3;

enum class ContentBoxKind : uint8_t {
  textBox = 0,
  valueBox = 1
};

enum class ContentType : uint8_t {
  text = 0,
  time = 1,
  date = 2,
  temperature = 3,
  humidity = 4
};

// Content cycle order follows the enum values up to this type.
constexpr ContentType lastContentType = ContentType::humidity;

constexpr uint8_t contentTypeMask(ContentType type)
{
  return static_cast<uint8_t>(1U << static_cast<uint8_t>(type));
}

constexpr uint8_t allCycleContentMask =
  contentTypeMask(ContentType::text)
  | contentTypeMask(ContentType::time)
  | contentTypeMask(ContentType::date)
  | contentTypeMask(ContentType::temperature)
  | contentTypeMask(ContentType::humidity);
constexpr uint16_t minimumCycleIntervalSeconds = 1;
constexpr uint16_t maximumCycleIntervalSeconds = 3600;
constexpr uint16_t defaultCycleIntervalSeconds = 5;

enum class ScrollDirection : uint8_t {
  left = 0,
  right = 1,
  up = 2,
  down = 3
};

enum class TextRotation : uint8_t {
  degrees0 = 0,
  degrees90 = 1,
  degrees180 = 2,
  degrees270 = 3
};

enum class ValueSource : uint8_t {
  modbus = 0,
  digitalInput = 1
};

enum class ModbusValueType : uint8_t {
  signedInteger16 = 0,
  ieee754Float = 1
};

enum class CounterDirection : uint8_t {
  up = 0,
  down = 1
};

// Number format of temperature and humidity content.
enum class TemperatureDisplayFormat : uint8_t {
  decimal = 0,
  integer = 1
};

// Area in which a box is clipped and its scrolling text wraps around.
enum class ScrollArea : uint8_t {
  panel = 0,
  display = 1
};

struct ContentItemSettings {
  ContentBoxKind boxKind = ContentBoxKind::textBox;
  uint8_t panelIndex = 0;
  ContentType type = ContentType::text;
  TextRotation rotation = TextRotation::degrees0;
  int16_t x = 0;
  int16_t y = 0;
  uint8_t red = 255;
  uint8_t green = 0;
  uint8_t blue = 0;
  bool scrolling = false;
  ScrollDirection direction = ScrollDirection::left;
  uint8_t speedPixelsPerSecond = 10;
  bool cycleEnabled = false;
  uint8_t cycleContentMask = contentTypeMask(ContentType::text);
  uint16_t cycleIntervalSeconds = defaultCycleIntervalSeconds;
  char text[maxContentTextLength] = {};
  ValueSource valueSource = ValueSource::modbus;
  ModbusValueType modbusValueType = ModbusValueType::signedInteger16;
  CounterDirection counterDirection = CounterDirection::up;
  uint16_t registerAddress = 0;
  int32_t initialIntegerValue = 0;
  float initialFloatValue = 0.0F;
  TemperatureDisplayFormat temperatureDisplayFormat =
    TemperatureDisplayFormat::decimal;
  uint8_t fontId = FontModel::defaultFontId;
  ScrollArea scrollArea = ScrollArea::panel;
  // Float: digits after the decimal point. Integer: fixed decimal point
  // position, e.g. 1234 with 2 decimals is shown as 12.34.
  uint8_t valueDecimals = 0;
};

struct ContentSettings {
  uint16_t schemaVersion = contentSchemaVersion;
  uint8_t count = 0;
  ContentItemSettings items[maxContentItems] = {};
};

