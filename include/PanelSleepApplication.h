#pragma once

class ClockApplication;
class PanelDisplay;
class SettingsService;

class PanelSleepApplication {
public:
  PanelSleepApplication(
    PanelDisplay &panel,
    ClockApplication &clock,
    SettingsService &settings);

  void begin();
  void update();
  void setTemporaryWakeOverride(bool enabled);
  bool sleeping() const;

private:
  bool shouldSleepNow() const;
  void applySchedule();

  PanelDisplay &panel_;
  ClockApplication &clock_;
  SettingsService &settings_;
  bool initialized_ = false;
  bool sleeping_ = false;
  bool temporaryWakeOverride_ = false;
};


