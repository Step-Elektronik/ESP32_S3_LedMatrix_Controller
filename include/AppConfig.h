#pragma once

#include <stddef.h>
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
#define FIRMWARE_VERSION "0.15.1"
#endif

namespace AppConfig {

constexpr uint32_t serialBaudRate = 115200;
constexpr char firmwareVersion[] = FIRMWARE_VERSION;

namespace Network {
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
// With "require login from the STA network" enabled in the Wi-Fi settings,
// requests arriving through the STA (LAN) interface must authenticate with
// HTTP Digest using this user name and the AP password. Clients of the local
// AP already proved knowledge of that password when they joined the AP.
constexpr char portalUser[] = "admin";
constexpr char portalRealm[] = "STEP-RGB";
} // namespace Network

namespace OnlineUpdate {
// GitHub Actions publishes every master build to the "firmware" branch of
// this repository (.github/workflows/firmware.yml). The repository must be
// public; the device has no GitHub credentials.
constexpr char repositoryName[] = "Step-Elektronik/ESP32_S3_LedMatrix_Controller";
constexpr char repositoryBaseUrl[] =
  "https://raw.githubusercontent.com/Step-Elektronik/ESP32_S3_LedMatrix_Controller/firmware/";
constexpr char manifestFile[] = "firmware.json";
// First automatic check after the STA link is up, then once a day; after a
// failed check the next try follows sooner.
constexpr uint32_t firstCheckDelayMs = 2UL * 60UL * 1000UL;
constexpr uint32_t checkIntervalMs = 24UL * 60UL * 60UL * 1000UL;
constexpr uint32_t retryIntervalMs = 60UL * 60UL * 1000UL;
constexpr uint32_t httpTimeoutMs = 15000;
// A TLS connection needs roughly 40 KB of internal RAM.
constexpr size_t minimumFreeHeap = 48UL * 1024UL;
} // namespace OnlineUpdate

namespace TimeSync {
constexpr char defaultNtpServer[] = "pool.ntp.org";
// POSIX TZ string for Turkey (UTC+3, no daylight saving time).
constexpr char defaultTimeZone[] = "<+03>-3";
constexpr uint32_t rtcWriteMinimumIntervalMs = 60000;
} // namespace TimeSync

namespace Hub75Pins {
// ESP32-S3 LCD_CAM/GDMA parallel output group. These pins are all exposed on
// the DevKitC-1 J1 header and avoid USB, UART0, strapping, RGB LED and
// flash/PSRAM-related pins. D and E are unused by the supported 1/4 and 1/8
// scan panels. The HUB12 header shares OE, A, B, C, CLK and LAT and has its
// R, G and D lines on r1, g1 and b1.
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
constexpr int8_t pin = 13;
constexpr uint8_t startupToggleCount = 6;
constexpr uint32_t startupToggleIntervalMs = 30;
} // namespace Buzzer

namespace StatusLeds {
constexpr int8_t accessPointClientPin = 14;
constexpr int8_t stationConnectedPin = 21;
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
constexpr size_t maximumFrameSize = 256;
// true: the UART drives DIR in hardware (RS485 half-duplex mode), so sending
// never blocks the main loop. false: DIR is toggled in software around a
// blocking flush, as in firmware 0.9.x.
constexpr bool hardwareDirectionControl = true;
constexpr size_t receiveBufferSize = 1024;
constexpr uint8_t frameQueueLength = 4;
// A request that waited longer than this in the queue is dropped instead of
// being answered late; a late reply could collide with the next bus cycle.
constexpr uint32_t maximumResponseDelayMs = 250;
} // namespace Modbus

namespace DigitalInput {
constexpr int8_t pin = 12;
constexpr uint32_t debounceMs = 30;
// The input is sampled by a hardware timer callback, so counting does not
// depend on how long the main loop is busy.
constexpr uint32_t samplePeriodUs = 1000;
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
constexpr size_t maxDeviceNameBytes = 32;
constexpr uint32_t bootStableDurationMs = 60000;
// New firmware runs on trial until it is confirmed: at least this long after
// boot and once a browser reached the portal, or after bootStableDurationMs.
constexpr uint32_t firmwareConfirmMinimumMs = 10000;
constexpr uint32_t rtcPollIntervalMs = 100;
constexpr uint8_t clockColorRed = 255;
constexpr uint8_t clockColorGreen = 0;
constexpr uint8_t clockColorBlue = 0;

} // namespace AppConfig
