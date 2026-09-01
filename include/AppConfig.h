#pragma once

#include <stdint.h>

// Select the physical panel scan type: 4 for 1/4 scan, 8 for 1/8 scan.
// It can also be supplied from platformio.ini:
// build_flags = -DPANEL_SCAN_DIVISOR=8
#ifndef PANEL_SCAN_DIVISOR
#define PANEL_SCAN_DIVISOR 4
#endif

#if PANEL_SCAN_DIVISOR != 4 && PANEL_SCAN_DIVISOR != 8
#error "PANEL_SCAN_DIVISOR must be 4 or 8"
#endif

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "0.9.0"
#endif

namespace AppConfig {

constexpr uint32_t serialBaudRate = 115200;
constexpr char firmwareVersion[] = FIRMWARE_VERSION;

namespace Network {
constexpr char hostname[] = "step-rgb";
constexpr char apSsid[] = "STEP-RGB";
constexpr char apPassword[] = "00000000";
constexpr uint8_t apChannel = 1;
constexpr uint8_t apMaxConnections = 4;
constexpr uint16_t dnsPort = 53;
constexpr uint16_t httpPort = 80;
constexpr uint32_t apRestartDelayMs = 500;
constexpr uint32_t apHealthCheckIntervalMs = 5000;
constexpr uint32_t staAttemptIntervalMs = 60000;
constexpr uint32_t staAttemptDurationMs = 10000;
} // namespace Network

namespace Hub75Pins {
// ESP32-S3 LCD_CAM/GDMA parallel output group. These pins are all exposed on
// the DevKitC-1 J1 header and avoid USB, UART0, strapping, RGB LED and
// flash/PSRAM-related pins. D and E are unused by the supported 1/4 and 1/8
// scan panels.
constexpr int8_t r1 = 4;
constexpr int8_t g1 = 5;
constexpr int8_t b1 = 6;
constexpr int8_t r2 = 7;
constexpr int8_t g2 = 15;
constexpr int8_t b2 = 16;
constexpr int8_t a = 17;
constexpr int8_t b = 18;
constexpr int8_t c = 8;
constexpr int8_t d = -1;
constexpr int8_t e = -1;
constexpr int8_t lat = 9;
constexpr int8_t oe = 10;
constexpr int8_t clk = 11;
} // namespace Hub75Pins

// Shared SHT40/DS3231 I2C bus on unrestricted J3 GPIOs.
constexpr int i2cSdaPin = 1;
constexpr int i2cSclPin = 2;
constexpr int rtcSdaPin = i2cSdaPin;
constexpr int rtcSclPin = i2cSclPin;

namespace Buzzer {
constexpr int8_t pin = 14;
constexpr uint8_t startupToggleCount = 6;
constexpr uint32_t startupToggleIntervalMs = 30;
} // namespace Buzzer

namespace StatusLeds {
constexpr int8_t accessPointClientPin = 12;
constexpr int8_t stationConnectedPin = 13;
constexpr uint32_t toggleIntervalMs = 1000;
} // namespace StatusLeds

namespace Alarm {
constexpr int8_t relayPin = 42;
constexpr bool relayActiveHigh = true;
} // namespace Alarm

namespace Modbus {
// UART1/RS-485 group on adjacent J3 pins. This consumes the external pad-JTAG
// pins, while the native USB Serial/JTAG interface on GPIO19/20 remains free.
constexpr int8_t rxPin = 39;
constexpr int8_t txPin = 40;
constexpr int8_t directionPin = 41;
constexpr uint8_t defaultSlaveAddress = 1;
constexpr uint32_t defaultBaudRate = 115200;
constexpr uint8_t maximumReadRegisters = 64;
} // namespace Modbus

namespace DigitalInput {
constexpr int8_t pin = 21;
constexpr uint32_t debounceMs = 30;
} // namespace DigitalInput

constexpr uint16_t panelWidth = 32;
constexpr uint16_t panelHeight = 16;
constexpr uint8_t panelRows = 1;
constexpr uint8_t panelColumns = 1;
constexpr uint8_t panelCount = panelRows * panelColumns;
constexpr uint16_t displayWidth = panelWidth * panelColumns;
constexpr uint16_t displayHeight = panelHeight * panelRows;
constexpr uint8_t panelBrightness = 64;
constexpr uint8_t panelTestMaximumBrightness = 32;
constexpr uint8_t maxPanelRows = 4;
constexpr uint8_t maxPanelColumns = 4;
constexpr uint8_t maxPanelCount = maxPanelRows * maxPanelColumns;

constexpr uint32_t startupColorDurationMs = 500;
constexpr uint32_t watchdogTimeoutMs = 20000;
constexpr uint8_t bootFailureLimit = 5;
constexpr uint32_t bootStableDurationMs = 60000;
constexpr uint32_t rtcPollIntervalMs = 100;
constexpr uint8_t clockColorRed = 255;
constexpr uint8_t clockColorGreen = 0;
constexpr uint8_t clockColorBlue = 0;

} // namespace AppConfig
