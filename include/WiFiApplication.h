#pragma once

#include <stdint.h>

#include <DNSServer.h>

class SettingsService;

enum class StationConnectionState : uint8_t {
  disabled,
  waiting,
  paused,
  connecting,
  connected,
  failed
};

enum class StationFailureReason : uint8_t {
  none,
  noSsid,
  authentication,
  connectionLost,
  timeout
};

class WiFiApplication {
public:
  explicit WiFiApplication(SettingsService &settings);

  bool begin();
  void update();
  uint8_t accessPointClientCount() const;
  bool stationHasIpAddress() const;
  StationConnectionState stationState() const;
  StationFailureReason stationFailureReason() const;
  String stationIpAddress() const;
  int32_t stationRssi() const;
  uint8_t stationSignalQuality() const;
  uint32_t stationRetrySeconds() const;
  // step-rgb-xxxx; also reachable as step-rgb-xxxx.local.
  const String &hostname() const;

private:
  bool startAccessPoint();
  bool startCaptiveDns();
  void startMdns();
  bool isAccessPointHealthy() const;
  void maintainStation();
  void startStationAttempt();
  void failStationAttempt(StationFailureReason reason);
  void scheduleAccessPointRestart();
  void maintainAccessPoint();
  void restartAccessPoint();
  void logNetworkChanges();

  SettingsService &settings_;
  DNSServer dnsServer_;
  String hostname_;
  bool ready_ = false;
  int lastStaStatus_ = -1;
  uint8_t lastApClientCount_ = 0;
  bool apRestartPending_ = false;
  uint32_t apRestartScheduledAt_ = 0;
  uint32_t lastApHealthCheckTime_ = 0;
  bool staConnecting_ = false;
  bool staWasConnected_ = false;
  bool staAttemptSuppressedByApClient_ = false;
  StationFailureReason staFailureReason_ = StationFailureReason::none;
  uint32_t staAttemptStartedAt_ = 0;
  uint32_t lastStaAttemptTime_ = 0;
};


