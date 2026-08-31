#include "WiFiApplication.h"

#include <Arduino.h>
#include <WiFi.h>

#include "AppConfig.h"
#include "SettingsService.h"

WiFiApplication::WiFiApplication(SettingsService &settings)
  : settings_(settings)
{
}

bool WiFiApplication::begin()
{
  WiFi.setHostname(AppConfig::Network::hostname);
  const bool wifiModeReady = WiFi.mode(WIFI_AP_STA);
  if (!wifiModeReady) {
    Serial.println("ERROR: WiFi AP+STA mode could not be enabled.");
  }
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(false, false);

  // Keep the STA interface available without connecting yet. Calling
  // WiFi.begin() without user-supplied credentials would reuse stale NVS
  // credentials and repeatedly scan for an unavailable network, disrupting
  // SoftAP discovery. STA connection will be started by the settings service
  // after valid credentials have been supplied.

  bool accessPointReady = false;
  if (wifiModeReady && startAccessPoint()) {
    accessPointReady = startCaptiveDns();
  }

  lastStaStatus_ = static_cast<int>(WiFi.status());
  lastApClientCount_ = WiFi.softAPgetStationNum();
  lastApHealthCheckTime_ = millis();
  lastStaAttemptTime_ = millis() - AppConfig::Network::staAttemptIntervalMs;
  ready_ = true;

  if (!accessPointReady) {
    scheduleAccessPointRestart();
    Serial.println("WiFi AP startup failed; automatic retry scheduled.");
    return false;
  }

  Serial.printf(
    "WiFi AP ready: %s | IP: %s\n",
    settings_.network().apSsid.c_str(),
    WiFi.softAPIP().toString().c_str()
  );
  Serial.printf("Portal: http://%s/\n", WiFi.softAPIP().toString().c_str());
  return true;
}

bool WiFiApplication::startAccessPoint()
{
  const IPAddress apAddress(192, 168, 4, 1);
  const IPAddress apGateway(192, 168, 4, 1);
  const IPAddress apSubnet(255, 255, 255, 0);
  if (!WiFi.softAPConfig(apAddress, apGateway, apSubnet)) {
    Serial.println("ERROR: WiFi AP network configuration failed.");
    return false;
  }

  if (!WiFi.softAP(
        settings_.network().apSsid.c_str(),
        settings_.network().apPassword.c_str(),
        AppConfig::Network::apChannel,
        false,
        AppConfig::Network::apMaxConnections)) {
    Serial.println("ERROR: WiFi access point could not be started.");
    return false;
  }

  if (!WiFi.setTxPower(WIFI_POWER_21dBm)) {
    Serial.println("ERROR: Maximum WiFi TX power could not be applied.");
  } else {
    const wifi_power_t appliedPower = WiFi.getTxPower();
    Serial.printf(
      "WiFi TX power: %.1f dBm (maximum requested).\n",
      static_cast<int>(appliedPower) / 4.0f);
  }
  return true;
}

bool WiFiApplication::startCaptiveDns()
{
  if (!dnsServer_.start(
        AppConfig::Network::dnsPort,
        String(),
        WiFi.softAPIP())) {
    Serial.println("ERROR: Captive portal DNS server could not be started.");
    return false;
  }
  return true;
}

bool WiFiApplication::isAccessPointHealthy() const
{
  const wifi_mode_t mode = WiFi.getMode();
  const bool apModeEnabled = mode == WIFI_AP || mode == WIFI_AP_STA;
  if (!apModeEnabled || WiFi.softAPIP() == IPAddress(0, 0, 0, 0)) {
    return false;
  }

  return WiFi.softAPSSID() == settings_.network().apSsid;
}

void WiFiApplication::update()
{
  if (!ready_) {
    return;
  }

  // Current ESP32 DNSServer is asynchronous; this call also preserves
  // compatibility with versions where DNS requests require polling.
  dnsServer_.processNextRequest();
  logNetworkChanges();
  maintainStation();
  maintainAccessPoint();
}

uint8_t WiFiApplication::accessPointClientCount() const
{
  return ready_ ? WiFi.softAPgetStationNum() : 0;
}

bool WiFiApplication::stationHasIpAddress() const
{
  return ready_
    && WiFi.status() == WL_CONNECTED
    && WiFi.localIP() != IPAddress(0, 0, 0, 0);
}

StationConnectionState WiFiApplication::stationState() const
{
  if (settings_.network().staSsid.isEmpty()) {
    return StationConnectionState::disabled;
  }
  if (WiFi.status() == WL_CONNECTED) {
    return StationConnectionState::connected;
  }
  if (staAttemptSuppressedByApClient_) {
    return StationConnectionState::paused;
  }
  if (staConnecting_) {
    return StationConnectionState::connecting;
  }
  if (staFailureReason_ != StationFailureReason::none) {
    return StationConnectionState::failed;
  }
  return StationConnectionState::waiting;
}

StationFailureReason WiFiApplication::stationFailureReason() const
{
  return staFailureReason_;
}

String WiFiApplication::stationIpAddress() const
{
  return WiFi.status() == WL_CONNECTED
    ? WiFi.localIP().toString()
    : String();
}

int32_t WiFiApplication::stationRssi() const
{
  return stationHasIpAddress() ? WiFi.RSSI() : 0;
}

uint8_t WiFiApplication::stationSignalQuality() const
{
  if (!stationHasIpAddress()) {
    return 0;
  }

  const int32_t rssi = WiFi.RSSI();
  if (rssi <= -100) {
    return 0;
  }
  if (rssi >= -50) {
    return 100;
  }
  return static_cast<uint8_t>(2 * (rssi + 100));
}

uint32_t WiFiApplication::stationRetrySeconds() const
{
  const StationConnectionState state = stationState();
  if (state != StationConnectionState::failed
      && state != StationConnectionState::waiting) {
    return 0;
  }

  const uint32_t elapsed = millis() - lastStaAttemptTime_;
  if (elapsed >= AppConfig::Network::staAttemptIntervalMs) {
    return 0;
  }

  const uint32_t remaining = AppConfig::Network::staAttemptIntervalMs - elapsed;
  return (remaining + 999U) / 1000U;
}

void WiFiApplication::maintainStation()
{
  const NetworkSettings &network = settings_.network();
  const uint32_t now = millis();

  if (network.staSsid.isEmpty()) {
    if (staConnecting_ || WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect(false, false);
    }
    staConnecting_ = false;
    staWasConnected_ = false;
    staAttemptSuppressedByApClient_ = false;
    staFailureReason_ = StationFailureReason::none;
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    staConnecting_ = false;
    staWasConnected_ = true;
    staAttemptSuppressedByApClient_ = false;
    staFailureReason_ = StationFailureReason::none;
    return;
  }

  const uint8_t apClientCount = WiFi.softAPgetStationNum();
  if (apClientCount > 0) {
    if (staConnecting_) {
      WiFi.disconnect(false, false);
      staConnecting_ = false;
      Serial.println("WiFi STA attempt cancelled to protect the active AP client.");
    }
    if (staWasConnected_) {
      staWasConnected_ = false;
      Serial.println("WiFi STA connection lost; retry paused while an AP client is active.");
    }
    if (!staAttemptSuppressedByApClient_) {
      Serial.println("WiFi STA attempts paused while an AP client is connected.");
    }
    staAttemptSuppressedByApClient_ = true;
    staFailureReason_ = StationFailureReason::none;
    return;
  }

  if (staAttemptSuppressedByApClient_) {
    staAttemptSuppressedByApClient_ = false;
    staFailureReason_ = StationFailureReason::none;
    lastStaAttemptTime_ = now - AppConfig::Network::staAttemptIntervalMs;
    Serial.println("WiFi AP client count is zero; STA attempts resumed immediately.");
  }

  if (staWasConnected_) {
    staWasConnected_ = false;
    staFailureReason_ = StationFailureReason::connectionLost;
    lastStaAttemptTime_ = now;
    // Restore the SoftAP home channel after the STA interface loses its
    // channel. Mode/IP checks can still look healthy while AP beacons have
    // actually stopped.
    scheduleAccessPointRestart();
  }

  if (staConnecting_) {
    const wl_status_t status = WiFi.status();
    if (status == WL_NO_SSID_AVAIL) {
      failStationAttempt(StationFailureReason::noSsid);
      return;
    }
    if (status == WL_CONNECT_FAILED) {
      failStationAttempt(StationFailureReason::authentication);
      return;
    }
    if (status == WL_CONNECTION_LOST) {
      failStationAttempt(StationFailureReason::connectionLost);
      return;
    }
    if (now - staAttemptStartedAt_ >= AppConfig::Network::staAttemptDurationMs) {
      failStationAttempt(StationFailureReason::timeout);
    }
    return;
  }

  if (now - lastStaAttemptTime_ >= AppConfig::Network::staAttemptIntervalMs) {
    startStationAttempt();
  }
}

void WiFiApplication::startStationAttempt()
{
  const NetworkSettings &network = settings_.network();
  lastStaAttemptTime_ = millis();
  staAttemptStartedAt_ = lastStaAttemptTime_;
  staConnecting_ = true;
  staFailureReason_ = StationFailureReason::none;

  Serial.printf("WiFi STA connecting to: %s\n", network.staSsid.c_str());
  if (network.staPassword.isEmpty()) {
    WiFi.begin(network.staSsid.c_str());
  } else {
    WiFi.begin(network.staSsid.c_str(), network.staPassword.c_str());
  }
}

void WiFiApplication::failStationAttempt(StationFailureReason reason)
{
  WiFi.disconnect(false, false);
  staConnecting_ = false;
  staWasConnected_ = false;
  staFailureReason_ = reason;
  // A failed full-channel STA scan can leave SoftAP configured but no longer
  // advertising. Force a delayed AP rebuild after every failed attempt.
  scheduleAccessPointRestart();
  Serial.println("WiFi STA connection failed; next attempt remains scheduled.");
}

void WiFiApplication::scheduleAccessPointRestart()
{
  if (!apRestartPending_) {
    apRestartPending_ = true;
    apRestartScheduledAt_ = millis();
  }
}

void WiFiApplication::maintainAccessPoint()
{
  const uint32_t now = millis();
  const uint8_t apClientCount = WiFi.softAPgetStationNum();

  // Never rebuild SoftAP while a client is actively using the local portal.
  // A connected client is also direct evidence that the AP is operational.
  if (apClientCount > 0) {
    if (apRestartPending_) {
      apRestartPending_ = false;
      Serial.println("WiFi AP restart cancelled because a client is connected.");
    }
    lastApHealthCheckTime_ = now;
    return;
  }

  if (now - lastApHealthCheckTime_
      >= AppConfig::Network::apHealthCheckIntervalMs) {
    lastApHealthCheckTime_ = now;
    if (!isAccessPointHealthy()) {
      scheduleAccessPointRestart();
    }
  }

  if (apRestartPending_
      && now - apRestartScheduledAt_ >= AppConfig::Network::apRestartDelayMs) {
    restartAccessPoint();
  }
}

void WiFiApplication::restartAccessPoint()
{
  apRestartPending_ = false;
  dnsServer_.stop();
  WiFi.softAPdisconnect(true);
  delay(100);

  if (!WiFi.mode(WIFI_AP_STA)) {
    Serial.println("ERROR: WiFi AP+STA mode could not be restored.");
    scheduleAccessPointRestart();
    return;
  }
  WiFi.setSleep(false);

  if (!startAccessPoint() || !startCaptiveDns()) {
    scheduleAccessPointRestart();
    return;
  }

  lastApClientCount_ = 0;
  Serial.printf(
    "WiFi AP restarted: %s | IP: %s\n",
    settings_.network().apSsid.c_str(),
    WiFi.softAPIP().toString().c_str()
  );
}

void WiFiApplication::logNetworkChanges()
{
  const int staStatus = static_cast<int>(WiFi.status());
  if (staStatus != lastStaStatus_) {
    lastStaStatus_ = staStatus;
    if (staStatus == WL_CONNECTED) {
      Serial.printf(
        "WiFi STA connected: %s | IP: %s\n",
        WiFi.SSID().c_str(),
        WiFi.localIP().toString().c_str()
      );
    } else {
      Serial.printf("WiFi STA status: %d\n", staStatus);
    }
  }

  const uint8_t apClientCount = WiFi.softAPgetStationNum();
  if (apClientCount != lastApClientCount_) {
    lastApClientCount_ = apClientCount;
    Serial.printf("WiFi AP clients: %u\n", apClientCount);
  }
}


