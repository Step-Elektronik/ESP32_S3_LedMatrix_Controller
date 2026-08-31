#pragma once

#include <stdint.h>

class WatchdogService {
public:
  bool begin(uint32_t timeoutMs);
  void feed();

private:
  bool active_ = false;
};


