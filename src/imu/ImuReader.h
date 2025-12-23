#pragma once

#include <M5Unified.h>
#include "MahonyAHRS.h"

struct ImuData {
    float accel[3];  // Accelerometer (g)
    float gyro[3];   // Gyroscope (rad/s)
    uint32_t timestamp;
};

struct Quaternion {
    float w, x, y, z;

    Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
};

class ImuReader {
public:
    ImuReader();

    bool begin();
    bool update();  // Returns true if new data available

    const ImuData& getData() const { return data_; }
    Quaternion getQuaternion() const;

    void setGyroOffset(float ox, float oy, float oz);
    void resetAHRS();

    // Auto-calibration: call repeatedly while device is stationary
    // Returns true when calibration is complete
    bool calibrateGyro();
    bool isCalibrated() const { return calibrated_; }

private:
    ImuData data_;
    MahonyAHRS ahrs_;
    uint32_t lastUpdateTime_;
    float gyroOffset_[3];
    bool calibrated_;

    // Calibration state
    static constexpr int CALIBRATION_SAMPLES = 100;
    int calibrationCount_;
    float calibrationSum_[3];
};
