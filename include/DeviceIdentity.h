#pragma once

#include <Arduino.h>

// Per-device identity derived from the factory Wi-Fi MAC address, so that
// many controllers on one network can be told apart without any setup.
namespace DeviceIdentity {

// Signature answered by /api/discover; the device manager page lists only
// devices that report exactly these values.
constexpr char signatureDevice[] = "STEP-RGB";
constexpr char signatureVendor[] = "Step Elektronik";
constexpr uint8_t discoveryProtocol = 1;

// 12 upper-case hex digits of the Wi-Fi station MAC, e.g. "A1B2C3D4E5F6".
String id();
// "step-rgb-e5f6": DHCP and mDNS host name (step-rgb-e5f6.local).
String hostname();
// "STEP-RGB-E5F6": device name until the user sets one.
String defaultName();

} // namespace DeviceIdentity
