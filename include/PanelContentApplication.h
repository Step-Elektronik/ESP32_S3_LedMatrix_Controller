#pragma once

#include <cstddef>
#include <stdint.h>

#include "ContentModel.h"

class Adafruit_GFX;
class ClockApplication;
class CountdownApplication;
class PanelDisplay;
class SettingsService;
class TemperatureApplication;
class ValueBoxApplication;

class PanelContentApplication {
public:
  PanelContentApplication(
    PanelDisplay &panel,
    ClockApplication &clock,
    TemperatureApplication &temperature,
    ValueBoxApplication &valueBoxes,
    CountdownApplication &countdown,
    SettingsService &settings);

  void begin();
  void update();
  void refresh();
  void reload();
  void setRenderingPaused(bool paused);
  bool renderingPaused() const;

private:
  struct ScrollRuntime {
    int16_t offsetX = 0;
    int16_t offsetY = 0;
    uint32_t lastStepTime = 0;
  };

  struct ItemRuntime {
    ScrollRuntime scroll;
    uint32_t lastCycleChangeTime = 0;
    ContentType activeType = ContentType::text;
  };

  // Pixel rectangle a box is clipped to; right and bottom are exclusive.
  struct ClipArea {
    int16_t left = 0;
    int16_t top = 0;
    int16_t right = 0;
    int16_t bottom = 0;
  };

  ClipArea clipArea(const ContentItemSettings &item) const;
  void draw();
  void updateCycle(
    uint8_t index,
    const ContentItemSettings &item,
    uint32_t now,
    bool &redrawRequired);
  void updateScroll(
    uint8_t index,
    const ContentItemSettings &item,
    uint32_t now,
    bool &redrawRequired);
  bool cycleDue(
    const ContentItemSettings &item,
    const ItemRuntime &runtime,
    uint32_t now) const;
  void buildText(
    uint8_t index,
    const ContentItemSettings &item,
    ContentType activeType,
    char *buffer,
    size_t bufferSize) const;
  void measureText(
    const char *text,
    TextRotation rotation,
    uint8_t fontId,
    uint16_t &width,
    uint16_t &height);
  void calculateAlignedOrigin(
    uint8_t index,
    const ContentItemSettings &item,
    ContentType activeType,
    uint16_t activeWidth,
    uint16_t activeHeight,
    int16_t &originX,
    int16_t &originY);
  void drawRotatedText(
    Adafruit_GFX &matrix,
    const char *text,
    int16_t x,
    int16_t y,
    uint16_t color,
    bool hideColon,
    TextRotation rotation,
    uint8_t fontId,
    const ClipArea &clip);

  PanelDisplay &panel_;
  ClockApplication &clock_;
  TemperatureApplication &temperature_;
  ValueBoxApplication &valueBoxes_;
  CountdownApplication &countdown_;
  SettingsService &settings_;
  ItemRuntime runtime_[maxContentItems] = {};
  uint8_t lastClockSecond_ = UINT8_MAX;
  uint8_t lastClockDay_ = UINT8_MAX;
  uint32_t lastTemperatureRevision_ = UINT32_MAX;
  uint32_t lastValueBoxRevision_ = UINT32_MAX;
  uint32_t lastCountdownRevision_ = UINT32_MAX;
  uint32_t renderingPausedAt_ = 0;
  bool renderingPaused_ = false;
  bool ready_ = false;
};

