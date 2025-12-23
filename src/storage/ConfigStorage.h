#pragma once

#include <Arduino.h>

#include <Preferences.h>

struct AppConfig {
    // WiFi settings
    String wifiSsid;
    String wifiPassword;

    // OSC settings
    String oscTargetIp;
    uint16_t oscSendPort;
    uint16_t oscRecvPort;

    // Gyro calibration
    float gyroOffsetX;
    float gyroOffsetY;
    float gyroOffsetZ;
    bool gyroCalibrated;

    AppConfig();
    void setDefaults();
};

class ConfigStorage {
public:
    ConfigStorage();

    bool begin();
    void end();

    bool load(AppConfig& config);
    bool save(const AppConfig& config);
    bool reset();

    // Save only gyro calibration (frequently updated)
    bool saveGyroCalibration(float ox, float oy, float oz);

private:
    Preferences prefs_;
    static constexpr const char* NAMESPACE = "twist-stick";
};
