#pragma once

#include <stdint.h>

class Adafruit_GFX;
class MatrixPanel_I2S_DMA;
struct PanelSettings;

class PanelDisplay {
public:
  bool begin(const PanelSettings &settings);
  void runStartupColorTest(uint32_t durationMs);
  void showSolidColor(uint8_t red, uint8_t green, uint8_t blue);
  void setBrightness(uint8_t brightness);
  void present();

  Adafruit_GFX &canvas();
  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) const;
  uint16_t width() const;
  uint16_t height() const;
  uint8_t brightness() const;
  bool ready() const;

private:
  MatrixPanel_I2S_DMA *dmaDisplay_ = nullptr;
  Adafruit_GFX *matrix_ = nullptr;
  uint16_t width_ = 0;
  uint16_t height_ = 0;
  uint8_t brightness_ = 0;
};


