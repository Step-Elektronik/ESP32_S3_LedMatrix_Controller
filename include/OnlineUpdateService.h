#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <atomic>
#include <mutex>
#include <stddef.h>
#include <stdint.h>

class BootGuardService;
class WiFiApplication;

enum class OnlineUpdateState : uint8_t {
  idle = 0,
  checking = 1,
  upToDate = 2,
  available = 3,
  downloading = 4,
  installed = 5,
  failed = 6
};

struct OnlineUpdateStatus {
  OnlineUpdateState state = OnlineUpdateState::idle;
  // A newer version was found; stays true after a failed installation so
  // it can be retried.
  bool updateAvailable = false;
  bool automaticCheck = true;
  bool stationConnected = false;
  uint8_t progress = 0;
  // -1 until the first check finished.
  int32_t secondsSinceCheck = -1;
  String latestVersion;
  String notes;
  uint32_t size = 0;
  String message;
  // Set when the previously installed version did not start and the
  // bootloader went back to this firmware.
  String rolledBackVersion;
};

// Checks the firmware that GitHub Actions publishes for every push to master
// (see .github/workflows/firmware.yml) and installs it only after the user
// confirms it in the portal. Network and flash work run in their own task so
// the panel and the portal keep running.
class OnlineUpdateService {
public:
  OnlineUpdateService(WiFiApplication &wifi, BootGuardService &bootGuard);

  void begin();
  void update();

  bool requestCheck(String &message);
  bool requestInstall(String &message);
  void setAutomaticCheck(bool enabled);
  bool busy() const;
  OnlineUpdateStatus status() const;

private:
  enum class Job : uint8_t {
    check,
    install
  };

  struct Manifest {
    String version;
    String file;
    String sha256;
    String notes;
    uint32_t size = 0;
  };

  static void taskEntry(void *context);
  bool startJob(Job job);
  void runCheck();
  void runInstall();
  bool fetchManifest(Manifest &manifest, String &error);
  void setState(OnlineUpdateState state, const String &message);

  WiFiApplication &wifi_;
  BootGuardService &bootGuard_;
  Preferences preferences_;
  bool storageReady_ = false;

  mutable std::mutex mutex_;
  // Guarded by mutex_.
  Manifest latest_;
  String message_;
  String rolledBackVersion_;

  std::atomic<OnlineUpdateState> state_{OnlineUpdateState::idle};
  std::atomic<uint8_t> progress_{0};
  std::atomic<bool> taskRunning_{false};
  std::atomic<bool> updateAvailable_{false};
  std::atomic<bool> installFinished_{false};
  std::atomic<bool> stationConnected_{false};
  std::atomic<uint32_t> lastCheckAt_{0};
  std::atomic<bool> checkedOnce_{false};
  std::atomic<bool> lastCheckFailed_{false};
  Job job_ = Job::check;

  bool automaticCheck_ = true;
  uint32_t connectedSince_ = 0;
  bool restartScheduled_ = false;
  uint32_t restartAt_ = 0;
  uint8_t buffer_[4096] = {};
};
