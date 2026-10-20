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
  // On single-colour panels every colour except black lights the LED.
  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) const;
  uint16_t width() const;
  uint16_t height() const;
  uint8_t brightness() const;
  bool ready() const;
  bool monochrome() const;

private:
  void configureHub12Signals(const PanelSettings &settings);

  MatrixPanel_I2S_DMA *dmaDisplay_ = nullptr;
  Adafruit_GFX *matrix_ = nullptr;
  bool monochrome_ = false;
  uint16_t width_ = 0;
  uint16_t height_ = 0;
  uint8_t brightness_ = 0;
};


