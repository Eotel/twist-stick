#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <functional>
#include "../storage/ConfigStorage.h"

class ConfigServer {
public:
    using ConfigCallback = std::function<void(const AppConfig&)>;

    ConfigServer();

    void begin();
    void stop();
    void handleClient();

    void setConfig(const AppConfig& config);
    void setConfigCallback(ConfigCallback callback);

    bool isRunning() const { return running_; }

private:
    WebServer server_;
    DNSServer dnsServer_;
    AppConfig config_;
    ConfigCallback callback_;
    bool running_;

    void handleRoot();
    void handleSave();
    void handleStatus();

    String generateHTML();
};
