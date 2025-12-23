#include "WifiManager.h"
#include "../config.h"

WifiManager::WifiManager()
    : mode_(Mode::DISCONNECTED)
    , apSsid_(AP_SSID) {
}

bool WifiManager::begin(const String& ssid, const String& password) {
    if (ssid.isEmpty()) {
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > WIFI_CONNECT_TIMEOUT_MS) {
            WiFi.disconnect();
            mode_ = Mode::DISCONNECTED;
            return false;
        }
        delay(100);
    }

    mode_ = Mode::STA;
    return true;
}

void WifiManager::startAP(const String& apSsid, const String& apPassword) {
    if (!apSsid.isEmpty()) {
        apSsid_ = apSsid;
    }

    WiFi.mode(WIFI_AP_STA);  // Allow both AP and STA for config while connected
    WiFi.softAP(apSsid_.c_str(), apPassword.isEmpty() ? nullptr : apPassword.c_str());

    mode_ = Mode::AP;
}

void WifiManager::stopAP() {
    WiFi.softAPdisconnect(true);
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.mode(WIFI_STA);
        mode_ = Mode::STA;
    } else {
        mode_ = Mode::DISCONNECTED;
    }
}

bool WifiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WifiManager::getLocalIP() const {
    return WiFi.localIP().toString();
}

String WifiManager::getAPIP() const {
    return WiFi.softAPIP().toString();
}

void WifiManager::disconnect() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    mode_ = Mode::DISCONNECTED;
}
