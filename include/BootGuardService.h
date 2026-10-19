#pragma once

#include <Preferences.h>
#include <stdint.h>

class BootGuardService {
public:
  bool begin();
  void update();
  void prepareForPlannedRestart();
  // A browser reached the portal, so Wi-Fi and the web server work.
  void reportPortalActivity();

  bool factoryResetRequired() const;
  // True while freshly installed firmware still runs on trial.
  bool firmwareAwaitingConfirmation() const;

private:
  bool resetCounter();
  // Cancels the bootloader rollback of a freshly installed firmware.
  void confirmRunningFirmware();

  Preferences preferences_;
  uint32_t bootStartedAt_ = 0;
  bool ready_ = false;
  bool counterCleared_ = false;
  bool firmwareConfirmed_ = false;
  bool portalActive_ = false;
  bool factoryResetRequired_ = false;
};


