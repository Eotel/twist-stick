#include "OscManager.h"
#include "../config.h"

OscManager::OscManager()
    : sendPort_(OSC_SEND_PORT)
    , recvPort_(OSC_RECV_PORT)
    , initialized_(false) {
}

void OscManager::begin(const String& targetIp, uint16_t sendPort, uint16_t recvPort) {
    targetIp_ = targetIp;
    sendPort_ = sendPort;
    recvPort_ = recvPort;

    // Setup OSC receiver for reset command
    OscWiFi.subscribe(recvPort_, OSC_RESET_ADDRESS,
        [this]() { onReset(); }
    );

    initialized_ = true;
}

void OscManager::setTarget(const String& targetIp, uint16_t sendPort) {
    targetIp_ = targetIp;
    sendPort_ = sendPort;
}

void OscManager::sendTwistData(const TwistData& data) {
    if (!initialized_ || targetIp_.isEmpty()) {
        return;
    }

    // Send all 9 values as floats
    // /twist-stick [angleX, angleY, angleZ, turnsX, turnsY, turnsZ, thetaX, thetaY, thetaZ]
    OscWiFi.send(targetIp_, sendPort_, OSC_ADDRESS,
        data.angle[0], data.angle[1], data.angle[2],
        (float)data.turns[0], (float)data.turns[1], (float)data.turns[2],
        data.theta[0], data.theta[1], data.theta[2]
    );
}

void OscManager::update() {
    if (!initialized_) {
        return;
    }
    OscWiFi.update();
}

void OscManager::setResetCallback(ResetCallback callback) {
    resetCallback_ = callback;
}

void OscManager::onReset() {
    if (resetCallback_) {
        resetCallback_();
    }
}
