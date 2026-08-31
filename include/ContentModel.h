#pragma once

#include <cstddef>
#include <stdint.h>

#include "AppConfig.h"
#include "FontModel.h"

constexpr uint16_t contentSchemaVersion = 6;
constexpr uint8_t maxBoxesPerPanel = 2;
constexpr uint8_t maxTextBoxesPerPanel = maxBoxesPerPanel;
constexpr uint8_t maxContentItems = AppConfig::maxPanelCount * maxBoxesPerPanel;
constexpr size_t maxContentTextLength = 48;

enum class ContentBoxKind : uint8_t {
  textBox = 0,
  valueBox = 1
};

enum class ContentType : uint8_t {
  text = 0,
  time = 1,
  date = 2,
  temperature = 3
};

constexpr uint8_t contentTypeMask(ContentType type)
{
  return static_cast<uint8_t>(1U << static_cast<uint8_t>(type));
}

constexpr uint8_t allCycleContentMask =
  contentTypeMask(ContentType::text)
  | contentTypeMask(ContentType::time)
  | contentTypeMask(ContentType::date)
  | contentTypeMask(ContentType::temperature);
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

enum class TemperatureDisplayFormat : uint8_t {
  decimal = 0,
  integer = 1
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
};

struct ContentSettings {
  uint16_t schemaVersion = contentSchemaVersion;
  uint8_t count = 0;
  ContentItemSettings items[maxContentItems] = {};
};

