#pragma once

#include <Preferences.h>
#include <stdint.h>

class BootGuardService {
public:
  bool begin();
  void update();
  void prepareForPlannedRestart();

  bool factoryResetRequired() const;

private:
  bool resetCounter();

  Preferences preferences_;
  uint32_t bootStartedAt_ = 0;
  bool ready_ = false;
  bool counterCleared_ = false;
  bool factoryResetRequired_ = false;
};


