#include "PanelDisplay.h"

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-HUB75-VirtualMatrixPanel_T.hpp>
#include <esp_rom_gpio.h>
#include <soc/gpio_sig_map.h>

#include "AppConfig.h"
#include "SettingsService.h"

namespace {

// LCD_CAM data lines used by the HUB75 DMA library (bus_cfg.pin_d0…d7).
constexpr uint32_t lcdDataR1 = 0;
constexpr uint32_t lcdDataG1 = 1;
constexpr uint32_t lcdDataOe = 7;

// On/off is all a single-colour panel shows; two bit planes keep the DMA
// buffer small and the refresh rate high.
constexpr uint8_t hub12ColorDepthBits = 2;

// The DMA library raises LAT together with the last pixel of a row, half a
// clock before that pixel is shifted in. HUB75 driver chips latch while LAT is
// high, but the 74HC595 on HUB12 modules latches on its rising edge and would
// miss the last bit, moving every pixel one register along (the rightmost
// pixel of each 8-pixel block then shows up four rows higher). The chain is
// therefore sent with one padding pixel before the data and one after it:
// LAT rides on the trailing pixel, and the even row width keeps the DMA
// buffers word aligned.
constexpr int16_t hub12LeadingPadding = 1;
constexpr uint16_t hub12PaddingPixels = 2;

// P10 single-colour HUB12 modules (32x16, 1/4 scan) shift 128 bits per scan
// line through one data input. For every 8-pixel column block the rows
// y+12, y+8, y+4 and y follow each other, 8 pixels each, leftmost pixel
// first (the order of the Freetronics DMD library). Address lines A/B select
// the scan line y (0–3). The DMA chain holds 128 pixels per module after the
// leading padding pixel, and only its upper half, shifted out on R1, is used.
struct Hub12P10Mapping {
  static VirtualCoords apply(VirtualCoords coords, int panelPixelBase)
  {
    constexpr int16_t blockWidth = 8;
    constexpr int16_t scanLines = 4;

    const int16_t panelIndex = coords.x / panelPixelBase;
    const int16_t panelX = coords.x % panelPixelBase;
    const int16_t rowGroup = coords.y / scanLines;

    coords.x = hub12LeadingPadding
             + panelIndex * (panelPixelBase * scanLines)
             + (panelX / blockWidth) * (blockWidth * scanLines)
             + (scanLines - 1 - rowGroup) * blockWidth
             + panelX % blockWidth;
    coords.y = coords.y % scanLines;
    return coords;
  }
};

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

  monochrome_ = settings.type == PanelType::monoHub12;

  // RGB 1/4 scan: two data lines (R1/R2) each carry two row groups, so a
  // module is 64x8 for the DMA. HUB12: one data line carries all four row
  // groups, so a module is 128 pixels wide; the lower half stays unused. The
  // HUB12 chain is configured as one wide panel so the padding pixels are
  // added once per row.
  uint16_t dmaWidth = AppConfig::panelWidth;
  uint16_t dmaHeight = AppConfig::panelHeight;
  uint16_t chainLength = settings.panelCount();
  if (monochrome_) {
    dmaWidth = AppConfig::panelWidth * 4 * settings.panelCount() + hub12PaddingPixels;
    dmaHeight = AppConfig::panelHeight / 2;
    chainLength = 1;
  } else if (settings.scanDivisor == 4) {
    dmaWidth = AppConfig::panelWidth * 2;
    dmaHeight = AppConfig::panelHeight / 2;
  }

  HUB75_I2S_CFG config(
    dmaWidth,
    dmaHeight,
    chainLength,
    pins
  );
  config.clkphase = false;
  config.double_buff = true;
  if (monochrome_) {
    config.setPixelColorDepthBits(hub12ColorDepthBits);
  }

  dmaDisplay_ = new MatrixPanel_I2S_DMA(config);
  if (dmaDisplay_ == nullptr || !dmaDisplay_->begin()) {
    Serial.println("ERROR: HUB75 DMA initialization failed.");
    return false;
  }
  if (monochrome_) {
    configureHub12Signals(settings);
  }

  setBrightness(settings.brightness);

  Adafruit_GFX *virtualPanelDisplay = nullptr;
  if (monochrome_) {
    virtualPanelDisplay = createConfiguredVirtualDisplay<Hub12P10Mapping>(
      settings,
      *dmaDisplay_);
  } else if (settings.scanDivisor == 4) {
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

  Serial.printf(
    "Panel type: %s\n",
    monochrome_ ? "single-colour HUB12" : "RGB HUB75");
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
  if (monochrome_) {
    // Only R1 reaches a HUB12 module; G1 and B1 (G and D) stay idle.
    return red == 0 && green == 0 && blue == 0
      ? 0
      : dmaDisplay_->color565(255, 0, 0);
  }
  return dmaDisplay_->color565(red, green, blue);
}

void PanelDisplay::configureHub12Signals(const PanelSettings &settings)
{
  // The DMA library drives HUB75 polarity: data high = LED on, OE low =
  // output on. Re-route the lines through the GPIO matrix with the inversion
  // a HUB12 module needs. G is inverted together with R so that it stays in
  // its "off" level on two-colour modules.
  esp_rom_gpio_connect_out_signal(
    AppConfig::Hub75Pins::r1,
    LCD_DATA_OUT0_IDX + lcdDataR1,
    settings.hub12DataInverted,
    false);
  esp_rom_gpio_connect_out_signal(
    AppConfig::Hub75Pins::g1,
    LCD_DATA_OUT0_IDX + lcdDataG1,
    settings.hub12DataInverted,
    false);
  esp_rom_gpio_connect_out_signal(
    AppConfig::Hub75Pins::oe,
    LCD_DATA_OUT0_IDX + lcdDataOe,
    settings.hub12OeInverted,
    false);
  Serial.printf(
    "HUB12 signals: data %s, OE %s.\n",
    settings.hub12DataInverted ? "active low" : "active high",
    settings.hub12OeInverted ? "active high" : "active low");
}

bool PanelDisplay::monochrome() const
{
  return monochrome_;
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


