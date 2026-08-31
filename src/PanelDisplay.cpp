#include "PanelDisplay.h"

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-HUB75-VirtualMatrixPanel_T.hpp>

#include "AppConfig.h"
#include "SettingsService.h"

namespace {

// P10RGB-3535-DP-4S uses an 8x4 Z-stripe layout.
struct P10Dp4SMapping {
  static VirtualCoords apply(VirtualCoords coords, int panelPixelBase)
  {
    constexpr int16_t tileWidth = 8;
    constexpr int16_t tileHeight = 4;

    const int16_t panelIndex = coords.x / panelPixelBase;
    const int16_t panelX = coords.x % panelPixelBase;
    const int16_t horizontalBlock = panelX / tileWidth;
    const int16_t xInBlock = panelX % tileWidth;
    const int16_t verticalBlock = coords.y / tileHeight;

    coords.x = panelIndex * (panelPixelBase * 2)
             + horizontalBlock * (tileWidth * 2)
             + (verticalBlock & 1) * tileWidth
             + xInBlock;
    coords.y = (coords.y % tileHeight)
             + (coords.y / (tileHeight * 2)) * tileHeight;
    return coords;
  }
};

template <PANEL_CHAIN_TYPE ChainType, class ScanMapping>
Adafruit_GFX *createVirtualDisplay(
  const PanelSettings &settings,
  MatrixPanel_I2S_DMA &dmaDisplay)
{
  auto *display = new VirtualMatrixPanel_T<ChainType, ScanMapping>(
    settings.rows,
    settings.columns,
    AppConfig::panelWidth,
    AppConfig::panelHeight
  );
  if (display != nullptr) {
    display->setDisplay(dmaDisplay);
  }
  return display;
}

template <class ScanMapping>
Adafruit_GFX *createConfiguredVirtualDisplay(
  const PanelSettings &settings,
  MatrixPanel_I2S_DMA &dmaDisplay)
{
  switch (settings.connectionStyle) {
    case PanelConnectionStyle::topLeftDown:
      return createVirtualDisplay<CHAIN_TOP_LEFT_DOWN, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::topRightDown:
      return createVirtualDisplay<CHAIN_TOP_RIGHT_DOWN, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::bottomLeftUp:
      return createVirtualDisplay<CHAIN_BOTTOM_LEFT_UP, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::bottomRightUp:
      return createVirtualDisplay<CHAIN_BOTTOM_RIGHT_UP, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::topLeftDownZigzag:
      return createVirtualDisplay<CHAIN_TOP_LEFT_DOWN_ZZ, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::topRightDownZigzag:
      return createVirtualDisplay<CHAIN_TOP_RIGHT_DOWN_ZZ, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::bottomRightUpZigzag:
      return createVirtualDisplay<CHAIN_BOTTOM_RIGHT_UP_ZZ, ScanMapping>(settings, dmaDisplay);
    case PanelConnectionStyle::bottomLeftUpZigzag:
      return createVirtualDisplay<CHAIN_BOTTOM_LEFT_UP_ZZ, ScanMapping>(settings, dmaDisplay);
    default:
      return nullptr;
  }
}

} // namespace

bool PanelDisplay::begin(const PanelSettings &settings)
{
  String validationError;
  if (!SettingsService::validatePanel(settings, validationError)) {
    Serial.printf("ERROR: Unsafe panel settings rejected: %s\n", validationError.c_str());
    return false;
  }

  HUB75_I2S_CFG::i2s_pins pins = {
    AppConfig::Hub75Pins::r1,
    AppConfig::Hub75Pins::g1,
    AppConfig::Hub75Pins::b1,
    AppConfig::Hub75Pins::r2,
    AppConfig::Hub75Pins::g2,
    AppConfig::Hub75Pins::b2,
    AppConfig::Hub75Pins::a,
    AppConfig::Hub75Pins::b,
    AppConfig::Hub75Pins::c,
    AppConfig::Hub75Pins::d,
    AppConfig::Hub75Pins::e,
    AppConfig::Hub75Pins::lat,
    AppConfig::Hub75Pins::oe,
    AppConfig::Hub75Pins::clk
  };

  const uint16_t dmaWidth = settings.scanDivisor == 4
    ? AppConfig::panelWidth * 2
    : AppConfig::panelWidth;
  const uint16_t dmaHeight = settings.scanDivisor == 4
    ? AppConfig::panelHeight / 2
    : AppConfig::panelHeight;

  HUB75_I2S_CFG config(
    dmaWidth,
    dmaHeight,
    settings.panelCount(),
    pins
  );
  config.clkphase = false;
  config.double_buff = true;

  dmaDisplay_ = new MatrixPanel_I2S_DMA(config);
  if (dmaDisplay_ == nullptr || !dmaDisplay_->begin()) {
    Serial.println("ERROR: HUB75 DMA initialization failed.");
    return false;
  }

  setBrightness(settings.brightness);

  Adafruit_GFX *virtualPanelDisplay = nullptr;
  if (settings.scanDivisor == 4) {
    virtualPanelDisplay = createConfiguredVirtualDisplay<P10Dp4SMapping>(
      settings,
      *dmaDisplay_);
  } else {
    virtualPanelDisplay = createConfiguredVirtualDisplay<
      ScanTypeMapping<STANDARD_TWO_SCAN>>(
        settings,
        *dmaDisplay_);
  }
  if (virtualPanelDisplay == nullptr) {
    Serial.println("ERROR: Virtual panel display allocation failed.");
    return false;
  }

  matrix_ = virtualPanelDisplay;
  width_ = AppConfig::panelWidth * settings.columns;
  height_ = AppConfig::panelHeight * settings.rows;

  Serial.printf("Panel scan mode: 1/%u\n", settings.scanDivisor);
  Serial.printf(
    "Panel connection style: %u\n",
    static_cast<unsigned>(settings.connectionStyle));
  Serial.printf(
    "Display layout: %ux%u panels, %ux%u pixels\n",
    settings.columns,
    settings.rows,
    width_,
    height_
  );
  return true;
}

void PanelDisplay::showSolidColor(
  uint8_t red,
  uint8_t green,
  uint8_t blue)
{
  matrix_->fillScreen(color565(red, green, blue));
  present();
}

void PanelDisplay::setBrightness(uint8_t brightness)
{
  if (dmaDisplay_ != nullptr) {
    dmaDisplay_->setBrightness8(brightness);
    brightness_ = brightness;
  }
}

void PanelDisplay::runStartupColorTest(uint32_t durationMs)
{
  matrix_->fillScreen(color565(255, 0, 0));
  present();
  delay(durationMs);
  matrix_->fillScreen(color565(0, 255, 0));
  present();
  delay(durationMs);
  matrix_->fillScreen(color565(0, 0, 255));
  present();
  delay(durationMs);
  matrix_->fillScreen(0);
  present();
}

void PanelDisplay::present()
{
  dmaDisplay_->flipDMABuffer();
}

Adafruit_GFX &PanelDisplay::canvas()
{
  return *matrix_;
}

uint16_t PanelDisplay::color565(uint8_t red, uint8_t green, uint8_t blue) const
{
  return dmaDisplay_->color565(red, green, blue);
}

uint16_t PanelDisplay::width() const
{
  return width_;
}

uint16_t PanelDisplay::height() const
{
  return height_;
}

uint8_t PanelDisplay::brightness() const
{
  return brightness_;
}

bool PanelDisplay::ready() const
{
  return dmaDisplay_ != nullptr && matrix_ != nullptr;
}


