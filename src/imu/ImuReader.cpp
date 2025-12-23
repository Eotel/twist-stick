#include "ImuReader.h"

#include "../config.h"

ImuReader::ImuReader()
    : ahrs_(MAHONY_KP, MAHONY_KI), lastUpdateTime_(0), calibrated_(false), calibrationCount_(0) {
    gyroOffset_[0] = gyroOffset_[1] = gyroOffset_[2] = 0.0f;
    calibrationSum_[0] = calibrationSum_[1] = calibrationSum_[2] = 0.0f;
}

bool ImuReader::begin() {
    // IMU is already initialized by M5.begin()
    if (!M5.Imu.isEnabled()) {
        return false;
    }
    lastUpdateTime_ = micros();
    return true;
}

bool ImuReader::update() {
    // Get IMU data using M5Unified API
    auto imuUpdate = M5.Imu.update();
    if (!imuUpdate) {
        return false;
    }

    // Read accelerometer (in G)
    M5.Imu.getAccel(&data_.accel[0], &data_.accel[1], &data_.accel[2]);

    // Read gyroscope (in deg/s, convert to rad/s)
    float gx = 0.0f;
    float gy = 0.0f;
    float gz = 0.0f;
    M5.Imu.getGyro(&gx, &gy, &gz);

    // DEG_TO_RAD is already defined in Arduino.h (as double)
    constexpr float DEG_TO_RAD_F = static_cast<float>(DEG_TO_RAD);
    data_.gyro[0] = (gx - gyroOffset_[0]) * DEG_TO_RAD_F;
    data_.gyro[1] = (gy - gyroOffset_[1]) * DEG_TO_RAD_F;
    data_.gyro[2] = (gz - gyroOffset_[2]) * DEG_TO_RAD_F;

    // Calculate delta time
    uint32_t now = micros();
    float dt = static_cast<float>(now - lastUpdateTime_) * 1e-6f;
    lastUpdateTime_ = now;

    // Clamp dt to reasonable range
    if (dt <= 0.0f || dt > 0.1f) {
        dt = 1.0f / IMU_SAMPLE_RATE;
    }

    data_.timestamp = now;

    // Update AHRS
    ahrs_.update(data_.accel[0], data_.accel[1], data_.accel[2], data_.gyro[0], data_.gyro[1],
                 data_.gyro[2], dt);

    return true;
}

Quaternion ImuReader::getQuaternion() const {
    Quaternion q;
    ahrs_.getQuaternion(q.w, q.x, q.y, q.z);
    return q;
}

void ImuReader::setGyroOffset(float ox, float oy, float oz) {
    gyroOffset_[0] = ox;
    gyroOffset_[1] = oy;
    gyroOffset_[2] = oz;
    calibrated_ = true;
}

void ImuReader::getGyroOffset(float& ox, float& oy, float& oz) const {
    ox = gyroOffset_[0];
    oy = gyroOffset_[1];
    oz = gyroOffset_[2];
}

void ImuReader::resetAHRS() {
    ahrs_.reset();
}

bool ImuReader::calibrateGyro() {
    if (calibrated_) {
        return true;
    }

    // Read raw gyro data
    float gx = 0.0f;
    float gy = 0.0f;
    float gz = 0.0f;
    M5.Imu.getGyro(&gx, &gy, &gz);

    calibrationSum_[0] += gx;
    calibrationSum_[1] += gy;
    calibrationSum_[2] += gz;
    calibrationCount_++;

    if (calibrationCount_ >= CALIBRATION_SAMPLES) {
        gyroOffset_[0] = calibrationSum_[0] / CALIBRATION_SAMPLES;
        gyroOffset_[1] = calibrationSum_[1] / CALIBRATION_SAMPLES;
        gyroOffset_[2] = calibrationSum_[2] / CALIBRATION_SAMPLES;
        calibrated_ = true;

        // Reset for potential recalibration
        calibrationCount_ = 0;
        calibrationSum_[0] = calibrationSum_[1] = calibrationSum_[2] = 0.0f;

        return true;
    }

    return false;
}

void ImuReader::startRecalibration() {
    calibrated_ = false;
    calibrationCount_ = 0;
    calibrationSum_[0] = calibrationSum_[1] = calibrationSum_[2] = 0.0f;
    gyroOffset_[0] = gyroOffset_[1] = gyroOffset_[2] = 0.0f;
}
