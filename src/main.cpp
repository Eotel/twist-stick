#include <M5Unified.h>
#include "config.h"
#include "imu/ImuReader.h"
#include "twist/TwistCalculator.h"
#include "osc/OscManager.h"
#include "network/WifiManager.h"
#include "network/ConfigServer.h"
#include "storage/ConfigStorage.h"
#include "display/DisplayManager.h"

// Global objects
ImuReader imuReader;
TwistCalculator twistCalculator;
OscManager oscManager;
WifiManager wifiManager;
ConfigServer configServer;
ConfigStorage configStorage;
DisplayManager display;
AppConfig appConfig;

// State
bool configMode = false;
uint32_t lastImuUpdate = 0;

void resetTwist() {
    twistCalculator.reset();
    Serial.println("Twist reset!");
}

void onConfigSaved(const AppConfig& newConfig) {
    appConfig = newConfig;
    configStorage.save(appConfig);
    Serial.println("Config saved, restarting...");
    delay(1000);
    ESP.restart();
}

void enterConfigMode() {
    configMode = true;
    wifiManager.startAP();
    configServer.setConfig(appConfig);
    configServer.setConfigCallback(onConfigSaved);
    configServer.begin();
    display.showConfigMode(wifiManager.getAPIP());
    Serial.println("Config mode started");
    Serial.print("AP IP: ");
    Serial.println(wifiManager.getAPIP());
}

void setup() {
    // Initialize M5 with all peripherals
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    Serial.println("\n=== TwistStick ===");

    // Initialize display
    display.begin();
    display.showBoot();

    // Initialize config storage
    if (!configStorage.begin()) {
        Serial.println("Failed to init NVS");
        display.showError("NVS Error");
        while (1) delay(1000);
    }
    configStorage.load(appConfig);

    // Initialize IMU
    if (!imuReader.begin()) {
        Serial.println("Failed to init IMU");
        display.showError("IMU Error");
        while (1) delay(1000);
    }

    // Load gyro calibration if available
    if (appConfig.gyroCalibrated) {
        imuReader.setGyroOffset(
            appConfig.gyroOffsetX,
            appConfig.gyroOffsetY,
            appConfig.gyroOffsetZ
        );
        Serial.println("Gyro calibration loaded");
    }

    // Calibrate gyro if not calibrated
    if (!imuReader.isCalibrated()) {
        Serial.println("Calibrating gyro...");
        int progress = 0;
        while (!imuReader.isCalibrated()) {
            M5.Imu.update();
            if (imuReader.calibrateGyro()) {
                break;
            }
            progress++;
            display.showCalibrating(progress);
            delay(10);
        }

        // Save calibration
        // Note: Gyro offset is stored internally in ImuReader as deg/s
        // We would need to expose it to save, but for now skip saving
        Serial.println("Gyro calibration complete");
    }

    // Check if config button is held during boot
    M5.update();
    if (M5.BtnA.isPressed()) {
        Serial.println("Button held, entering config mode...");
        enterConfigMode();
        return;
    }

    // Try to connect to WiFi
    if (!appConfig.wifiSsid.isEmpty()) {
        display.showConnecting(appConfig.wifiSsid);
        Serial.print("Connecting to WiFi: ");
        Serial.println(appConfig.wifiSsid);

        if (wifiManager.begin(appConfig.wifiSsid, appConfig.wifiPassword)) {
            Serial.print("Connected! IP: ");
            Serial.println(wifiManager.getLocalIP());

            // Initialize OSC
            oscManager.begin(appConfig.oscTargetIp, appConfig.oscSendPort, appConfig.oscRecvPort);
            oscManager.setResetCallback(resetTwist);

            Serial.print("OSC target: ");
            Serial.print(appConfig.oscTargetIp);
            Serial.print(":");
            Serial.println(appConfig.oscSendPort);
        } else {
            Serial.println("WiFi connection failed, entering config mode");
            enterConfigMode();
            return;
        }
    } else {
        Serial.println("No WiFi configured, entering config mode");
        enterConfigMode();
        return;
    }

    // Initialize twist calculator
    twistCalculator.reset();

    // Ensure display is awake and ready
    display.wake();

    Serial.println("Setup complete!");
}

void loop() {
    M5.update();

    // Handle config mode
    if (configMode) {
        configServer.handleClient();
        display.update();

        // Long press to exit config mode and restart
        if (M5.BtnA.pressedFor(3000)) {
            ESP.restart();
        }
        return;
    }

    // Wake display on any button press
    if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed()) {
        display.wake();
    }

    // Button A: Reset twist angles
    if (M5.BtnA.wasPressed()) {
        resetTwist();
    }

    // Button B (if available): Enter config mode (long press)
    if (M5.BtnB.pressedFor(2000)) {
        enterConfigMode();
        return;
    }

    // Update IMU at target rate
    uint32_t now = micros();
    if (now - lastImuUpdate >= IMU_UPDATE_INTERVAL_US) {
        lastImuUpdate = now;

        if (imuReader.update()) {
            // Get quaternion and calculate twist
            Quaternion q = imuReader.getQuaternion();
            TwistData twist = twistCalculator.step(q);

            // Send via OSC
            if (wifiManager.isConnected()) {
                oscManager.sendTwistData(twist);
                oscManager.sendQuaternion(q);
            }

            // Update display
            display.showRunning(wifiManager.getLocalIP(), twist);
        }
    }

    // Process incoming OSC messages
    oscManager.update();

    // Update display
    display.update();

    // Small delay to prevent tight loop
    delayMicroseconds(100);
}
