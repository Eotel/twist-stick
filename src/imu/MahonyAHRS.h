#pragma once

// Mahony AHRS algorithm
// Based on: https://github.com/naninunenoy/AxisOrange
// Original: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/

class MahonyAHRS {
public:
    MahonyAHRS(float kp = 0.5f, float ki = 0.0f);

    // Update quaternion from 6DOF sensor data
    // accel: accelerometer data (any consistent units, will be normalized)
    // gyro: gyroscope data in rad/s
    // dt: delta time in seconds
    void update(float ax, float ay, float az,
                float gx, float gy, float gz,
                float dt);

    // Get quaternion (w, x, y, z)
    void getQuaternion(float& w, float& x, float& y, float& z) const;

    // Reset to identity quaternion
    void reset();

    // Set filter gains
    void setGains(float kp, float ki);

private:
    float q0, q1, q2, q3;  // Quaternion
    float integralFBx, integralFBy, integralFBz;  // Integral feedback
    float Kp, Ki;  // Filter gains

    static float invSqrt(float x);
};
