#include "DeviceIdentity.h"

#include <esp_mac.h>

namespace {

void readMac(uint8_t mac[6])
{
  // Works before Wi-Fi is started; the eFuse MAC never changes.
  if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
    memset(mac, 0, 6);
  }
}

String hexSuffix(bool upperCase)
{
  uint8_t mac[6];
  readMac(mac);
  char text[5];
  snprintf(text, sizeof(text), upperCase ? "%02X%02X" : "%02x%02x", mac[4], mac[5]);
  return String(text);
}

} // namespace

namespace DeviceIdentity {

String id()
{
  uint8_t mac[6];
  readMac(mac);
  char text[13];
  snprintf(
    text,
    sizeof(text),
    "%02X%02X%02X%02X%02X%02X",
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(text);
}

String hostname()
{
  return String("step-rgb-") + hexSuffix(false);
}

String defaultName()
{
  return String("STEP-RGB-") + hexSuffix(true);
}

} // namespace DeviceIdentity
