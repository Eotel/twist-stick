#pragma once

#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
public:
    enum class Mode {
        DISCONNECTED,
        STA,
        AP
    };

    WifiManager();

    // Try to connect to saved WiFi, returns true if connected
    bool begin(const String& ssid, const String& password);

    // Start AP mode for configuration
    void startAP(const String& apSsid = "", const String& apPassword = "");

    // Stop AP mode
    void stopAP();

    // Check connection status
    bool isConnected() const;
    Mode getMode() const { return mode_; }

    // Get IP address
    String getLocalIP() const;
    String getAPIP() const;

    // Disconnect and cleanup
    void disconnect();

private:
    Mode mode_;
    String apSsid_;
};
