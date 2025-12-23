#pragma once

#include <M5Unified.h>
#include "../twist/TwistCalculator.h"

class DisplayManager {
public:
    enum class Screen {
        BOOT,
        CALIBRATING,
        CONNECTING,
        CONFIG_MODE,
        RUNNING,
        ERROR
    };

    DisplayManager();

    void begin();
    void update();

    void setScreen(Screen screen);
    Screen getScreen() const { return screen_; }

    void showBoot();
    void showCalibrating(int progress);
    void showConnecting(const String& ssid);
    void showConfigMode(const String& apIp);
    void showRunning(const String& ip, const TwistData& data);
    void showError(const String& message);

private:
    Screen screen_;
    uint32_t lastUpdate_;
    bool needsRedraw_;

    void clear();
    void drawHeader(const char* title);
};
