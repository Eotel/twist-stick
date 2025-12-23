#include "MahonyAHRS.h"

#include <cmath>

MahonyAHRS::MahonyAHRS(float kp, float ki)
    : q0_(1.0f), q1_(0.0f), q2_(0.0f), q3_(0.0f), integralFBx_(0.0f), integralFBy_(0.0f),
      integralFBz_(0.0f), kp_(kp), ki_(ki) {}

void MahonyAHRS::update(float ax, float ay, float az, float gx, float gy, float gz, float dt) {
    float recipNorm;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float qa, qb, qc;

    // Compute feedback only if accelerometer measurement valid
    // (avoids NaN in accelerometer normalisation)
    if ((ax != 0.0f) || (ay != 0.0f) || (az != 0.0f)) {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Estimated direction of gravity
        halfvx = q1_ * q3_ - q0_ * q2_;
        halfvy = q0_ * q1_ + q2_ * q3_;
        halfvz = q0_ * q0_ - 0.5f + q3_ * q3_;

        // Error is sum of cross product between estimated and measured direction
        halfex = (ay * halfvz - az * halfvy);
        halfey = (az * halfvx - ax * halfvz);
        halfez = (ax * halfvy - ay * halfvx);

        // Compute and apply integral feedback if enabled
        if (ki_ > 0.0f) {
            integralFBx_ += ki_ * halfex * dt;
            integralFBy_ += ki_ * halfey * dt;
            integralFBz_ += ki_ * halfez * dt;
            gx += integralFBx_;
            gy += integralFBy_;
            gz += integralFBz_;
        } else {
            integralFBx_ = 0.0f;
            integralFBy_ = 0.0f;
            integralFBz_ = 0.0f;
        }

        // Apply proportional feedback
        gx += kp_ * halfex;
        gy += kp_ * halfey;
        gz += kp_ * halfez;
    }

    // Integrate rate of change of quaternion
    gx *= 0.5f * dt;
    gy *= 0.5f * dt;
    gz *= 0.5f * dt;
    qa = q0_;
    qb = q1_;
    qc = q2_;
    q0_ += (-qb * gx - qc * gy - q3_ * gz);
    q1_ += (qa * gx + qc * gz - q3_ * gy);
    q2_ += (qa * gy - qb * gz + q3_ * gx);
    q3_ += (qa * gz + qb * gy - qc * gx);

    // Normalise quaternion
    recipNorm = invSqrt(q0_ * q0_ + q1_ * q1_ + q2_ * q2_ + q3_ * q3_);
    q0_ *= recipNorm;
    q1_ *= recipNorm;
    q2_ *= recipNorm;
    q3_ *= recipNorm;
}

void MahonyAHRS::getQuaternion(float& w, float& x, float& y, float& z) const {
    w = q0_;
    x = q1_;
    y = q2_;
    z = q3_;
}

void MahonyAHRS::reset() {
    q0_ = 1.0f;
    q1_ = q2_ = q3_ = 0.0f;
    integralFBx_ = integralFBy_ = integralFBz_ = 0.0f;
}

void MahonyAHRS::setGains(float kp, float ki) {
    kp_ = kp;
    ki_ = ki;
}

// Fast inverse square root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float MahonyAHRS::invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    y = y * (1.5f - (halfx * y * y));  // Second iteration for better accuracy
    return y;
}
