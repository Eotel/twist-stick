#include "ConfigServer.h"
#include <WiFi.h>

ConfigServer::ConfigServer()
    : server_(80)
    , running_(false) {
}

void ConfigServer::begin() {
    // Setup DNS for captive portal
    dnsServer_.start(53, "*", WiFi.softAPIP());

    // Setup web server routes
    server_.on("/", HTTP_GET, [this]() { handleRoot(); });
    server_.on("/save", HTTP_POST, [this]() { handleSave(); });
    server_.on("/status", HTTP_GET, [this]() { handleStatus(); });
    server_.onNotFound([this]() { handleRoot(); });  // Captive portal redirect

    server_.begin();
    running_ = true;
}

void ConfigServer::stop() {
    server_.stop();
    dnsServer_.stop();
    running_ = false;
}

void ConfigServer::handleClient() {
    if (!running_) return;
    dnsServer_.processNextRequest();
    server_.handleClient();
}

void ConfigServer::setConfig(const AppConfig& config) {
    config_ = config;
}

void ConfigServer::setConfigCallback(ConfigCallback callback) {
    callback_ = callback;
}

void ConfigServer::handleRoot() {
    server_.send(200, "text/html", generateHTML());
}

void ConfigServer::handleSave() {
    if (server_.hasArg("ssid")) {
        config_.wifiSsid = server_.arg("ssid");
    }
    if (server_.hasArg("password")) {
        config_.wifiPassword = server_.arg("password");
    }
    if (server_.hasArg("osc_ip")) {
        config_.oscTargetIp = server_.arg("osc_ip");
    }
    if (server_.hasArg("osc_send")) {
        config_.oscSendPort = server_.arg("osc_send").toInt();
    }
    if (server_.hasArg("osc_recv")) {
        config_.oscRecvPort = server_.arg("osc_recv").toInt();
    }

    if (callback_) {
        callback_(config_);
    }

    server_.send(200, "text/html",
        "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>Saved</title></head><body style='font-family:sans-serif;text-align:center;padding:20px'>"
        "<h2>Settings Saved!</h2><p>Device will restart...</p>"
        "<script>setTimeout(()=>location='/',5000)</script>"
        "</body></html>"
    );
}

void ConfigServer::handleStatus() {
    String json = "{";
    json += "\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";
    json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"ssid\":\"" + config_.wifiSsid + "\"";
    json += "}";
    server_.send(200, "application/json", json);
}

String ConfigServer::generateHTML() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>TwistStick Config</title>
    <style>
        *{box-sizing:border-box}
        body{font-family:sans-serif;margin:0;padding:20px;background:#1a1a2e;color:#eee}
        .container{max-width:400px;margin:0 auto}
        h1{color:#0ff;text-align:center;margin-bottom:30px}
        .form-group{margin-bottom:15px}
        label{display:block;margin-bottom:5px;color:#aaa}
        input{width:100%;padding:12px;border:1px solid #333;border-radius:4px;background:#16213e;color:#fff}
        input:focus{outline:none;border-color:#0ff}
        button{width:100%;padding:15px;background:#0ff;color:#000;border:none;border-radius:4px;font-size:16px;cursor:pointer;margin-top:20px}
        button:hover{background:#00d4d4}
        .section{background:#16213e;padding:20px;border-radius:8px;margin-bottom:20px}
        .section h2{margin:0 0 15px 0;font-size:16px;color:#0ff}
    </style>
</head>
<body>
    <div class="container">
        <h1>TwistStick</h1>
        <form action="/save" method="POST">
            <div class="section">
                <h2>WiFi Settings</h2>
                <div class="form-group">
                    <label>SSID</label>
                    <input type="text" name="ssid" value=")";
    html += config_.wifiSsid;
    html += R"(" placeholder="WiFi network name">
                </div>
                <div class="form-group">
                    <label>Password</label>
                    <input type="password" name="password" value=")";
    html += config_.wifiPassword;
    html += R"(" placeholder="WiFi password">
                </div>
            </div>
            <div class="section">
                <h2>OSC Settings</h2>
                <div class="form-group">
                    <label>Target IP</label>
                    <input type="text" name="osc_ip" value=")";
    html += config_.oscTargetIp;
    html += R"(" placeholder="192.168.1.100">
                </div>
                <div class="form-group">
                    <label>Send Port</label>
                    <input type="number" name="osc_send" value=")";
    html += String(config_.oscSendPort);
    html += R"(">
                </div>
                <div class="form-group">
                    <label>Receive Port</label>
                    <input type="number" name="osc_recv" value=")";
    html += String(config_.oscRecvPort);
    html += R"(">
                </div>
            </div>
            <button type="submit">Save & Restart</button>
        </form>
    </div>
</body>
</html>
)";
    return html;
}
