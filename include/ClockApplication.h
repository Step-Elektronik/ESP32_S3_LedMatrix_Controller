#pragma once

#include "ClockService.h"

class ClockApplication {
public:
  void begin();
  void update();
  ClockDateTime dateTime() const;
  bool setDateTime(const ClockDateTime &dateTime);
  bool ready() const;

private:
  ClockService clock_;
  ClockDateTime currentDateTime_ = {0, 0, 0, 0, 0, 0};
  uint32_t lastRtcPollTime_ = 0;
  bool ready_ = false;
};


