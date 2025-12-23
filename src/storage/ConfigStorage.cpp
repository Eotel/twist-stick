#include "ConfigStorage.h"
#include "../config.h"

AppConfig::AppConfig() {
    setDefaults();
}

void AppConfig::setDefaults() {
    wifiSsid = "";
    wifiPassword = "";
    oscTargetIp = "192.168.1.100";
    oscSendPort = OSC_SEND_PORT;
    oscRecvPort = OSC_RECV_PORT;
    gyroOffsetX = 0.0f;
    gyroOffsetY = 0.0f;
    gyroOffsetZ = 0.0f;
    gyroCalibrated = false;
}

ConfigStorage::ConfigStorage() {}

bool ConfigStorage::begin() {
    return prefs_.begin(NAMESPACE, false);
}

void ConfigStorage::end() {
    prefs_.end();
}

bool ConfigStorage::load(AppConfig& config) {
    config.wifiSsid = prefs_.getString("wifi_ssid", "");
    config.wifiPassword = prefs_.getString("wifi_pass", "");
    config.oscTargetIp = prefs_.getString("osc_ip", "192.168.1.100");
    config.oscSendPort = prefs_.getUShort("osc_send", OSC_SEND_PORT);
    config.oscRecvPort = prefs_.getUShort("osc_recv", OSC_RECV_PORT);
    config.gyroOffsetX = prefs_.getFloat("gyro_ox", 0.0f);
    config.gyroOffsetY = prefs_.getFloat("gyro_oy", 0.0f);
    config.gyroOffsetZ = prefs_.getFloat("gyro_oz", 0.0f);
    config.gyroCalibrated = prefs_.getBool("gyro_cal", false);

    return true;
}

bool ConfigStorage::save(const AppConfig& config) {
    bool ok = true;
    ok &= prefs_.putString("wifi_ssid", config.wifiSsid);
    ok &= prefs_.putString("wifi_pass", config.wifiPassword);
    ok &= prefs_.putString("osc_ip", config.oscTargetIp);
    ok &= prefs_.putUShort("osc_send", config.oscSendPort);
    ok &= prefs_.putUShort("osc_recv", config.oscRecvPort);
    ok &= prefs_.putFloat("gyro_ox", config.gyroOffsetX);
    ok &= prefs_.putFloat("gyro_oy", config.gyroOffsetY);
    ok &= prefs_.putFloat("gyro_oz", config.gyroOffsetZ);
    ok &= prefs_.putBool("gyro_cal", config.gyroCalibrated);

    return ok;
}

bool ConfigStorage::reset() {
    return prefs_.clear();
}

bool ConfigStorage::saveGyroCalibration(float ox, float oy, float oz) {
    bool ok = true;
    ok &= prefs_.putFloat("gyro_ox", ox);
    ok &= prefs_.putFloat("gyro_oy", oy);
    ok &= prefs_.putFloat("gyro_oz", oz);
    ok &= prefs_.putBool("gyro_cal", true);
    return ok;
}
