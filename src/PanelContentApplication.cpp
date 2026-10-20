#include "PanelContentApplication.h"

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <FontCatalog.h>
#include <cmath>

#include "ClockApplication.h"
#include "CountdownApplication.h"
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
       type <= static_cast<uint8_t>(lastCycleContentType);
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
       type <= static_cast<uint8_t>(lastCycleContentType);
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
      % (static_cast<uint8_t>(lastCycleContentType) + 1U));
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
  CountdownApplication &countdown,
  SettingsService &settings)
  : panel_(panel),
    clock_(clock),
    temperature_(temperature),
    valueBoxes_(valueBoxes),
    countdown_(countdown),
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
  const uint32_t countdownRevision = countdown_.revision();

  for (uint8_t index = 0; index < content.count; ++index) {
    const ContentItemSettings &item = content.items[index];
    updateScroll(index, item, now, redrawRequired);
    updateCycle(index, item, now, redrawRequired);
    const ContentType activeType = runtime_[index].activeType;

    if (activeType == ContentType::time && dateTime.second != lastClockSecond_) {
      redrawRequired = true;
    }
    if (activeType == ContentType::date && dateTime.day != lastClockDay_) {
      redrawRequired = true;
    }
    if ((activeType == ContentType::temperature
         || activeType == ContentType::humidity)
        && temperatureRevision != lastTemperatureRevision_) {
      redrawRequired = true;
    }
    if (activeType == ContentType::countdown
        && countdownRevision != lastCountdownRevision_) {
      redrawRequired = true;
    }
  }

  lastClockSecond_ = dateTime.second;
  lastClockDay_ = dateTime.day;
  lastTemperatureRevision_ = temperatureRevision;
  lastCountdownRevision_ = countdownRevision;
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
  lastCountdownRevision_ = UINT32_MAX;
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

PanelContentApplication::ClipArea PanelContentApplication::clipArea(
  const ContentItemSettings &item) const
{
  ClipArea area;
  if (item.scrollArea == ScrollArea::panel) {
    const uint8_t columns = settings_.panel().columns;
    area.left = (item.panelIndex % columns) * AppConfig::panelWidth;
    area.top = (item.panelIndex / columns) * AppConfig::panelHeight;
    area.right = area.left + AppConfig::panelWidth;
    area.bottom = area.top + AppConfig::panelHeight;
  } else {
    area.right = static_cast<int16_t>(panel_.width());
    area.bottom = static_cast<int16_t>(panel_.height());
  }
  return area;
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
      item.fontId,
      clipArea(item));
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
    redrawRequired = true;
    return;
  }

  // A scrolling box switches content in updateScroll, while its text is out
  // of sight, so the next content scrolls in instead of jumping into place.
  if (item.scrolling) {
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

  runtime.activeType = advanceCycleType(
    item.cycleContentMask,
    runtime.activeType,
    1);
  runtime.lastCycleChangeTime = now;
  runtime.scroll = {};
  runtime.scroll.lastStepTime = now;
  redrawRequired = true;
}

bool PanelContentApplication::cycleDue(
  const ContentItemSettings &item,
  const ItemRuntime &runtime,
  uint32_t now) const
{
  if (item.boxKind != ContentBoxKind::textBox
      || !item.cycleEnabled
      || !cycleContains(item.cycleContentMask, runtime.activeType)
      || cycleTypeCount(item.cycleContentMask) < 2) {
    return false;
  }
  const uint32_t interval =
    static_cast<uint32_t>(item.cycleIntervalSeconds) * 1000U;
  return now - runtime.lastCycleChangeTime >= interval;
}

void PanelContentApplication::updateScroll(
  uint8_t index,
  const ContentItemSettings &item,
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

  int16_t textWidth = 0;
  int16_t textHeight = 0;
  int16_t originX = item.x;
  int16_t originY = item.y;
  const auto measureActive = [&]() {
    char text[64] = {};
    buildText(index, item, itemRuntime.activeType, text, sizeof(text));
    uint16_t width = 0;
    uint16_t height = 0;
    measureText(text, item.rotation, item.fontId, width, height);
    originX = item.x;
    originY = item.y;
    calculateAlignedOrigin(
      index,
      item,
      itemRuntime.activeType,
      width,
      height,
      originX,
      originY);
    textWidth = static_cast<int16_t>(width);
    textHeight = static_cast<int16_t>(height);
  };
  measureActive();

  // Text leaves the box's area on one side and re-enters on the other: the
  // own panel, or the whole display for boxes set to use it.
  const ClipArea area = clipArea(item);
  bool leftArea = false;
  switch (item.direction) {
    case ScrollDirection::left:
      runtime.offsetX -= steps;
      leftArea = originX + runtime.offsetX + textWidth < area.left;
      break;
    case ScrollDirection::right:
      runtime.offsetX += steps;
      leftArea = originX + runtime.offsetX >= area.right;
      break;
    case ScrollDirection::up:
      runtime.offsetY -= steps;
      leftArea = originY + runtime.offsetY + textHeight < area.top;
      break;
    case ScrollDirection::down:
      runtime.offsetY += steps;
      leftArea = originY + runtime.offsetY >= area.bottom;
      break;
  }

  if (leftArea) {
    // The next cycle content enters from the edge where the text would
    // have re-entered, so the change happens without a jump.
    if (cycleDue(item, itemRuntime, now)) {
      itemRuntime.activeType = advanceCycleType(
        item.cycleContentMask,
        itemRuntime.activeType,
        1);
      itemRuntime.lastCycleChangeTime = now;
      measureActive();
    }
    switch (item.direction) {
      case ScrollDirection::left:
        runtime.offsetX = area.right - originX;
        break;
      case ScrollDirection::right:
        runtime.offsetX = area.left - textWidth - originX;
        break;
      case ScrollDirection::up:
        runtime.offsetY = area.bottom - originY;
        break;
      case ScrollDirection::down:
        runtime.offsetY = area.top - textHeight - originY;
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
    // The unit suffix may contain Turkish letters or the degree sign.
    char rawText[64] = {};
    valueBoxes_.formatValue(index, rawText, sizeof(rawText));
    FontCatalog::encodeText(item.fontId, rawText, buffer, bufferSize);
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
        // "TMP?", "SHT?" or "HDC?" names the sensor that is missing.
        strlcpy(buffer, temperature_.missingText(), bufferSize);
      }
      break;
    case ContentType::countdown:
      countdown_.format(item, buffer, bufferSize);
      break;
    case ContentType::humidity:
      if (!temperature_.humiditySupported()) {
        // The selected sensor (TMP112) has no humidity channel.
        strlcpy(buffer, "--%", bufferSize);
      } else if (temperature_.ready()) {
        if (item.temperatureDisplayFormat == TemperatureDisplayFormat::integer) {
          snprintf(
            buffer,
            bufferSize,
            "%ld%%",
            std::lround(temperature_.relativeHumidity()));
        } else {
          snprintf(
            buffer,
            bufferSize,
            "%.1f%%",
            static_cast<double>(temperature_.relativeHumidity()));
        }
      } else {
        strlcpy(buffer, temperature_.missingText(), bufferSize);
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
  if (!item.cycleEnabled) {
    return;
  }

  // Cycled types are centred on the box of one reference type: the box's own
  // type while it is part of the cycle, otherwise the first cycled type, so
  // that a type that is never shown cannot move the others.
  const ContentType reference = cycleContains(item.cycleContentMask, item.type)
    ? item.type
    : firstCycleType(item.cycleContentMask, item.type);
  if (activeType == reference) {
    return;
  }

  char referenceText[64] = {};
  buildText(index, item, reference, referenceText, sizeof(referenceText));
  uint16_t referenceWidth = 0;
  uint16_t referenceHeight = 0;
  measureText(
    referenceText,
    item.rotation,
    item.fontId,
    referenceWidth,
    referenceHeight);

  // Only the visible part of the reference box counts, so a long (scrolling)
  // text does not push the other types out of view.
  const ClipArea area = clipArea(item);
  const int32_t left = item.x > area.left ? item.x : area.left;
  const int32_t right = item.x + static_cast<int32_t>(referenceWidth) < area.right
    ? item.x + static_cast<int32_t>(referenceWidth)
    : area.right;
  const int32_t top = item.y > area.top ? item.y : area.top;
  const int32_t bottom = item.y + static_cast<int32_t>(referenceHeight) < area.bottom
    ? item.y + static_cast<int32_t>(referenceHeight)
    : area.bottom;
  if (right > left) {
    originX = static_cast<int16_t>(left + (right - left - activeWidth) / 2);
  }
  if (bottom > top) {
    originY = static_cast<int16_t>(top + (bottom - top - activeHeight) / 2);
  }
}

void PanelContentApplication::drawRotatedText(
  Adafruit_GFX &matrix,
  const char *text,
  int16_t x,
  int16_t y,
  uint16_t color,
  bool hideColon,
  TextRotation rotation,
  uint8_t fontId,
  const ClipArea &clip)
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
            const int16_t pixelX = x + rotatedX;
            const int16_t pixelY = y + rotatedY;
            if (pixelX >= clip.left && pixelX < clip.right
                && pixelY >= clip.top && pixelY < clip.bottom) {
              matrix.drawPixel(pixelX, pixelY, color);
            }
          }
          bits <<= 1;
        }
      }
    }

    cursorX += glyph.xAdvance;
  }
}
