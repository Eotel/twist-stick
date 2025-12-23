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

    // Power saving
    void wake();
    void sleep();
    bool isSleeping() const { return sleeping_; }
    void resetTimeout();

private:
    Screen screen_;
    uint32_t lastUpdate_;
    uint32_t lastActivity_;
    bool needsRedraw_;
    bool sleeping_;

    void clear();
    void drawHeader(const char* title);
    void checkTimeout();
};
