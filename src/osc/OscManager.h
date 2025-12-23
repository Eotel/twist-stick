#pragma once

#include <Arduino.h>
#include <ArduinoOSCWiFi.h>

#include <functional>

#include "../imu/ImuReader.h"
#include "../twist/TwistCalculator.h"

class OscManager {
public:
    using ResetCallback = std::function<void()>;
    using CalibrateCallback = std::function<void()>;

    OscManager();

    void begin(const String& targetIp, uint16_t sendPort, uint16_t recvPort);
    void setTarget(const String& targetIp, uint16_t sendPort);

    void sendTwistData(const TwistData& data);
    void sendQuaternion(const Quaternion& q);
    void sendStatus(const char* status);
    void update();  // Process incoming messages

    void setResetCallback(ResetCallback callback);
    void setCalibrateCallback(CalibrateCallback callback);

private:
    String targetIp_;
    uint16_t sendPort_;
    uint16_t recvPort_;
    ResetCallback resetCallback_;
    CalibrateCallback calibrateCallback_;
    bool initialized_;

    void onReset();
    void onCalibrate();
};
