#include "PanelContentApplication.h"

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <FontCatalog.h>
#include <cmath>

#include "ClockApplication.h"
#include "PanelDisplay.h"
#include "SettingsService.h"
#include "TemperatureApplication.h"
#include "ValueBoxApplication.h"

namespace {

bool cycleContains(uint8_t mask, ContentType type)
{
  return (mask & contentTypeMask(type)) != 0;
}

uint8_t cycleTypeCount(uint8_t mask)
{
  uint8_t count = 0;
  for (uint8_t type = 0;
       type <= static_cast<uint8_t>(ContentType::temperature);
       ++type) {
    if (cycleContains(mask, static_cast<ContentType>(type))) {
      ++count;
    }
  }
  return count;
}

ContentType firstCycleType(uint8_t mask, ContentType fallback)
{
  for (uint8_t type = 0;
       type <= static_cast<uint8_t>(ContentType::temperature);
       ++type) {
    const ContentType candidate = static_cast<ContentType>(type);
    if (cycleContains(mask, candidate)) {
      return candidate;
    }
  }
  return fallback;
}

ContentType advanceCycleType(
  uint8_t mask,
  ContentType current,
  uint32_t steps)
{
  const uint8_t selectedCount = cycleTypeCount(mask);
  if (selectedCount == 0) {
    return current;
  }

  steps %= selectedCount;
  uint8_t type = static_cast<uint8_t>(current);
  while (steps > 0) {
    type = static_cast<uint8_t>(
      (type + 1U)
      % (static_cast<uint8_t>(ContentType::temperature) + 1U));
    if (cycleContains(mask, static_cast<ContentType>(type))) {
      --steps;
    }
  }
  return static_cast<ContentType>(type);
}

} // namespace

PanelContentApplication::PanelContentApplication(
  PanelDisplay &panel,
  ClockApplication &clock,
  TemperatureApplication &temperature,
  ValueBoxApplication &valueBoxes,
  SettingsService &settings)
  : panel_(panel),
    clock_(clock),
    temperature_(temperature),
    valueBoxes_(valueBoxes),
    settings_(settings)
{
}

void PanelContentApplication::begin()
{
  if (!panel_.ready()) {
    return;
  }

  Adafruit_GFX &matrix = panel_.canvas();
  matrix.setFont(FontCatalog::font(FontModel::defaultFontId));
  matrix.setTextSize(1);
  matrix.setTextWrap(false);

  ready_ = true;
  reload();
}

void PanelContentApplication::update()
{
  if (!ready_ || renderingPaused_) {
    return;
  }

  const uint32_t now = millis();
  bool redrawRequired = false;
  const ContentSettings &content = settings_.content();
  const ClockDateTime dateTime = clock_.dateTime();
  const uint32_t temperatureRevision = temperature_.revision();
  const uint32_t valueBoxRevision = valueBoxes_.revision();

  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    updateScroll(
      index,
      item,
      runtime_[index].activeType,
      now,
      redrawRequired);
    updateCycle(index, item, now, redrawRequired);
    const ContentType activeType = runtime_[index].activeType;

    if (activeType == ContentType::time && dateTime.second != lastClockSecond_) {
      redrawRequired = true;
    }
    if (activeType == ContentType::date && dateTime.day != lastClockDay_) {
      redrawRequired = true;
    }
    if (activeType == ContentType::temperature
        && temperatureRevision != lastTemperatureRevision_) {
      redrawRequired = true;
    }
  }

  lastClockSecond_ = dateTime.second;
  lastClockDay_ = dateTime.day;
  lastTemperatureRevision_ = temperatureRevision;
  if (valueBoxRevision != lastValueBoxRevision_) {
    redrawRequired = true;
  }
  lastValueBoxRevision_ = valueBoxRevision;
  if (redrawRequired) {
    draw();
  }
}

void PanelContentApplication::refresh()
{
  if (ready_ && !renderingPaused_) {
    draw();
  }
}

void PanelContentApplication::reload()
{
  const uint32_t now = millis();
  if (renderingPaused_) {
    renderingPausedAt_ = now;
  }
  const ContentSettings &content = settings_.content();
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    ItemRuntime &runtime = runtime_[index];
    runtime = {};
    runtime.scroll.lastStepTime = now;
    runtime.lastCycleChangeTime = now;
    if (index < content.count) {
      const ContentItemSettings &item = content.items[index];
      runtime.activeType = item.boxKind == ContentBoxKind::valueBox
        ? ContentType::text
        : item.cycleEnabled
        ? firstCycleType(item.cycleContentMask, item.type)
        : item.type;
    }
  }
  lastClockSecond_ = UINT8_MAX;
  lastClockDay_ = UINT8_MAX;
  lastTemperatureRevision_ = UINT32_MAX;
  lastValueBoxRevision_ = UINT32_MAX;
  refresh();
}

void PanelContentApplication::setRenderingPaused(bool paused)
{
  if (!ready_ || paused == renderingPaused_) {
    return;
  }

  const uint32_t now = millis();
  if (paused) {
    renderingPausedAt_ = now;
    renderingPaused_ = true;
    return;
  }

  const uint32_t pausedDuration = now - renderingPausedAt_;
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    runtime_[index].scroll.lastStepTime += pausedDuration;
    runtime_[index].lastCycleChangeTime += pausedDuration;
  }
  renderingPaused_ = false;
}

bool PanelContentApplication::renderingPaused() const
{
  return renderingPaused_;
}

void PanelContentApplication::draw()
{
  Adafruit_GFX &matrix = panel_.canvas();
  matrix.fillScreen(0);
  matrix.setFont(FontCatalog::font(FontModel::defaultFontId));
  matrix.setTextSize(1);
  matrix.setTextWrap(false);

  const ContentSettings &content = settings_.content();
  const ClockDateTime dateTime = clock_.dateTime();
  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    const ContentType activeType = runtime_[index].activeType;
    const ScrollRuntime &scroll = runtime_[index].scroll;
    char text[64] = {};
    buildText(index, item, activeType, text, sizeof(text));

    int16_t xOffset = 0;
    int16_t yOffset = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    FontCatalog::getTextBounds(
      item.fontId,
      text,
      xOffset,
      yOffset,
      width,
      height);

    uint16_t visualWidth = width;
    uint16_t visualHeight = height;
    if (item.rotation == TextRotation::degrees90
        || item.rotation == TextRotation::degrees270) {
      visualWidth = height;
      visualHeight = width;
    }
    int16_t originX = item.x;
    int16_t originY = item.y;
    calculateAlignedOrigin(
      index,
      item,
      activeType,
      visualWidth,
      visualHeight,
      originX,
      originY);

    const uint16_t color = panel_.color565(item.red, item.green, item.blue);
    const bool hideColon = item.boxKind == ContentBoxKind::textBox
      && activeType == ContentType::time
      && clock_.ready()
      && (dateTime.second & 1) != 0;

    drawRotatedText(
      matrix,
      text,
      originX + scroll.offsetX,
      originY + scroll.offsetY,
      color,
      hideColon,
      item.rotation,
      item.fontId);
  }

  panel_.present();
}

void PanelContentApplication::updateCycle(
  uint8_t index,
  const ContentItemSettings &item,
  uint32_t now,
  bool &redrawRequired)
{
  ItemRuntime &runtime = runtime_[index];
  if (item.boxKind == ContentBoxKind::valueBox) {
    runtime.activeType = ContentType::text;
    runtime.lastCycleChangeTime = now;
    return;
  }
  if (!item.cycleEnabled) {
    runtime.activeType = item.type;
    runtime.lastCycleChangeTime = now;
    return;
  }

  if (!cycleContains(item.cycleContentMask, runtime.activeType)) {
    runtime.activeType = firstCycleType(item.cycleContentMask, item.type);
    runtime.lastCycleChangeTime = now;
    runtime.scroll = {};
    runtime.scroll.lastStepTime = now;
    runtime.scrollWrapped = false;
    runtime.scrollTourCompleted = false;
    redrawRequired = true;
    return;
  }

  const uint32_t interval =
    static_cast<uint32_t>(item.cycleIntervalSeconds) * 1000U;
  const uint32_t elapsed = now - runtime.lastCycleChangeTime;
  if (elapsed < interval) {
    return;
  }

  if (cycleTypeCount(item.cycleContentMask) < 2) {
    runtime.lastCycleChangeTime = now;
    return;
  }
  if (item.scrolling && !runtime.scrollTourCompleted) {
    return;
  }

  runtime.activeType = advanceCycleType(
    item.cycleContentMask,
    runtime.activeType,
    1);
  runtime.lastCycleChangeTime = now;
  runtime.scroll = {};
  runtime.scroll.lastStepTime = now;
  runtime.scrollWrapped = false;
  runtime.scrollTourCompleted = false;
  redrawRequired = true;
}

void PanelContentApplication::updateScroll(
  uint8_t index,
  const ContentItemSettings &item,
  ContentType activeType,
  uint32_t now,
  bool &redrawRequired)
{
  if (!item.scrolling) {
    return;
  }

  ItemRuntime &itemRuntime = runtime_[index];
  ScrollRuntime &runtime = itemRuntime.scroll;
  const uint32_t interval = 1000U / item.speedPixelsPerSecond;
  const uint32_t elapsed = now - runtime.lastStepTime;
  if (elapsed < interval) {
    return;
  }

  const int16_t steps = static_cast<int16_t>(elapsed / interval);
  runtime.lastStepTime += static_cast<uint32_t>(steps) * interval;

  char text[64] = {};
  buildText(index, item, activeType, text, sizeof(text));
  uint16_t textWidth = 0;
  uint16_t textHeight = 0;
  measureText(text, item.rotation, item.fontId, textWidth, textHeight);
  int16_t originX = item.x;
  int16_t originY = item.y;
  calculateAlignedOrigin(
    index,
    item,
    activeType,
    textWidth,
    textHeight,
    originX,
    originY);

  switch (item.direction) {
    case ScrollDirection::left: {
      const int16_t previousOffset = runtime.offsetX;
      runtime.offsetX -= steps;
      if (itemRuntime.scrollWrapped
          && previousOffset > 0
          && runtime.offsetX <= 0) {
        itemRuntime.scrollTourCompleted = true;
      }
      if (originX + runtime.offsetX + static_cast<int16_t>(textWidth) < 0) {
        runtime.offsetX = panel_.width() - originX;
        itemRuntime.scrollWrapped = true;
      }
      break;
    }
    case ScrollDirection::right: {
      const int16_t previousOffset = runtime.offsetX;
      runtime.offsetX += steps;
      if (itemRuntime.scrollWrapped
          && previousOffset < 0
          && runtime.offsetX >= 0) {
        itemRuntime.scrollTourCompleted = true;
      }
      if (originX + runtime.offsetX >= static_cast<int16_t>(panel_.width())) {
        runtime.offsetX = -static_cast<int16_t>(textWidth) - originX;
        itemRuntime.scrollWrapped = true;
      }
      break;
    }
    case ScrollDirection::up: {
      const int16_t previousOffset = runtime.offsetY;
      runtime.offsetY -= steps;
      if (itemRuntime.scrollWrapped
          && previousOffset > 0
          && runtime.offsetY <= 0) {
        itemRuntime.scrollTourCompleted = true;
      }
      if (originY + runtime.offsetY + static_cast<int16_t>(textHeight) < 0) {
        runtime.offsetY = panel_.height() - originY;
        itemRuntime.scrollWrapped = true;
      }
      break;
    }
    case ScrollDirection::down: {
      const int16_t previousOffset = runtime.offsetY;
      runtime.offsetY += steps;
      if (itemRuntime.scrollWrapped
          && previousOffset < 0
          && runtime.offsetY >= 0) {
        itemRuntime.scrollTourCompleted = true;
      }
      if (originY + runtime.offsetY >= static_cast<int16_t>(panel_.height())) {
        runtime.offsetY = -static_cast<int16_t>(textHeight) - originY;
        itemRuntime.scrollWrapped = true;
      }
      break;
    }
  }
  redrawRequired = true;
}

void PanelContentApplication::buildText(
  uint8_t index,
  const ContentItemSettings &item,
  ContentType activeType,
  char *buffer,
  size_t bufferSize) const
{
  if (item.boxKind == ContentBoxKind::valueBox) {
    valueBoxes_.formatValue(index, buffer, bufferSize);
    return;
  }

  const ClockDateTime dateTime = clock_.dateTime();
  switch (activeType) {
    case ContentType::time:
      if (clock_.ready()) {
        snprintf(buffer, bufferSize, "%02u:%02u", dateTime.hour, dateTime.minute);
      } else {
        strlcpy(buffer, "RTC?", bufferSize);
      }
      break;
    case ContentType::date:
      if (clock_.ready()) {
        snprintf(
          buffer,
          bufferSize,
          "%02u/%02u/%02u",
          dateTime.day,
          dateTime.month,
          dateTime.year % 100);
      } else {
        strlcpy(buffer, "RTC?", bufferSize);
      }
      break;
    case ContentType::temperature:
      if (temperature_.ready()) {
        char rawText[64] = {};
        if (item.temperatureDisplayFormat
            == TemperatureDisplayFormat::integer) {
          snprintf(
            rawText,
            sizeof(rawText),
            "%ld\xC2\xB0" "C",
            std::lround(temperature_.temperatureC()));
        } else {
          snprintf(
            rawText,
            sizeof(rawText),
            "%.1f\xC2\xB0" "C",
            static_cast<double>(temperature_.temperatureC()));
        }
        FontCatalog::encodeText(
          item.fontId,
          rawText,
          buffer,
          bufferSize);
      } else {
        strlcpy(buffer, "TMP?", bufferSize);
      }
      break;
    case ContentType::text:
    default:
      FontCatalog::encodeText(
        item.fontId,
        item.text,
        buffer,
        bufferSize);
      break;
  }
}

void PanelContentApplication::measureText(
  const char *text,
  TextRotation rotation,
  uint8_t fontId,
  uint16_t &width,
  uint16_t &height)
{
  int16_t xOffset = 0;
  int16_t yOffset = 0;
  FontCatalog::getTextBounds(
    fontId,
    text,
    xOffset,
    yOffset,
    width,
    height);

  if (rotation == TextRotation::degrees90
      || rotation == TextRotation::degrees270) {
    const uint16_t originalWidth = width;
    width = height;
    height = originalWidth;
  }
}

void PanelContentApplication::calculateAlignedOrigin(
  uint8_t index,
  const ContentItemSettings &item,
  ContentType activeType,
  uint16_t activeWidth,
  uint16_t activeHeight,
  int16_t &originX,
  int16_t &originY)
{
  originX = item.x;
  originY = item.y;
  if (!item.cycleEnabled || activeType == item.type) {
    return;
  }

  char referenceText[64] = {};
  buildText(index, item, item.type, referenceText, sizeof(referenceText));
  uint16_t referenceWidth = 0;
  uint16_t referenceHeight = 0;
  measureText(
    referenceText,
    item.rotation,
    item.fontId,
    referenceWidth,
    referenceHeight);

  originX += static_cast<int16_t>(
    (static_cast<int32_t>(referenceWidth) - activeWidth) / 2);
  originY += static_cast<int16_t>(
    (static_cast<int32_t>(referenceHeight) - activeHeight) / 2);
}

void PanelContentApplication::drawRotatedText(
  Adafruit_GFX &matrix,
  const char *text,
  int16_t x,
  int16_t y,
  uint16_t color,
  bool hideColon,
  TextRotation rotation,
  uint8_t fontId)
{
  int16_t boundsX = 0;
  int16_t boundsY = 0;
  uint16_t sourceWidth = 0;
  uint16_t sourceHeight = 0;
  FontCatalog::getTextBounds(
    fontId,
    text,
    boundsX,
    boundsY,
    sourceWidth,
    sourceHeight);
  if (sourceWidth == 0 || sourceHeight == 0) {
    return;
  }

  int16_t cursorX = 0;
  int16_t cursorY = 0;
  uint8_t generatedBitmap[FontCatalog::generatedGlyphBufferSize] = {};

  for (const uint8_t *character =
         reinterpret_cast<const uint8_t *>(text);
       *character != 0;
       ++character) {
    const uint8_t code = *character;
    if (code == '\n') {
      cursorX = 0;
      cursorY += FontCatalog::yAdvance(fontId);
      continue;
    }
    if (code == '\r') {
      continue;
    }

    FontCatalog::GlyphView glyph;
    if (!FontCatalog::glyph(
          fontId,
          code,
          generatedBitmap,
          sizeof(generatedBitmap),
          glyph)) {
      continue;
    }
    size_t bitmapOffset = 0;

    if (!(hideColon && code == ':')) {
      uint8_t bits = 0;
      uint8_t bitIndex = 0;
      for (uint8_t sourceY = 0; sourceY < glyph.height; ++sourceY) {
        for (uint8_t sourceX = 0; sourceX < glyph.width; ++sourceX) {
          if ((bitIndex++ & 7) == 0) {
            const uint8_t *byteAddress = &glyph.bitmap[bitmapOffset++];
            bits = glyph.bitmapInProgramMemory
              ? pgm_read_byte(byteAddress)
              : *byteAddress;
          }
          if ((bits & 0x80) != 0) {
            const int16_t localX =
              cursorX + glyph.xOffset + sourceX - boundsX;
            const int16_t localY =
              cursorY + glyph.yOffset + sourceY - boundsY;
            int16_t rotatedX = localX;
            int16_t rotatedY = localY;
            switch (rotation) {
              case TextRotation::degrees90:
                rotatedX = static_cast<int16_t>(sourceHeight) - 1 - localY;
                rotatedY = localX;
                break;
              case TextRotation::degrees180:
                rotatedX = static_cast<int16_t>(sourceWidth) - 1 - localX;
                rotatedY = static_cast<int16_t>(sourceHeight) - 1 - localY;
                break;
              case TextRotation::degrees270:
                rotatedX = localY;
                rotatedY = static_cast<int16_t>(sourceWidth) - 1 - localX;
                break;
              case TextRotation::degrees0:
              default:
                break;
            }
            matrix.drawPixel(x + rotatedX, y + rotatedY, color);
          }
          bits <<= 1;
        }
      }
    }

    cursorX += glyph.xAdvance;
  }
}
