#include "MahonyAHRS.h"
#include <cmath>

MahonyAHRS::MahonyAHRS(float kp, float ki)
    : q0(1.0f), q1(0.0f), q2(0.0f), q3(0.0f)
    , integralFBx(0.0f), integralFBy(0.0f), integralFBz(0.0f)
    , Kp(kp), Ki(ki) {
}

void MahonyAHRS::update(float ax, float ay, float az,
                        float gx, float gy, float gz,
                        float dt) {
    float recipNorm;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float qa, qb, qc;

    // Compute feedback only if accelerometer measurement valid
    // (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Estimated direction of gravity
        halfvx = q1 * q3 - q0 * q2;
        halfvy = q0 * q1 + q2 * q3;
        halfvz = q0 * q0 - 0.5f + q3 * q3;

        // Error is sum of cross product between estimated and measured direction
        halfex = (ay * halfvz - az * halfvy);
        halfey = (az * halfvx - ax * halfvz);
        halfez = (ax * halfvy - ay * halfvx);

        // Compute and apply integral feedback if enabled
        if (Ki > 0.0f) {
            integralFBx += Ki * halfex * dt;
            integralFBy += Ki * halfey * dt;
            integralFBz += Ki * halfez * dt;
            gx += integralFBx;
            gy += integralFBy;
            gz += integralFBz;
        } else {
            integralFBx = 0.0f;
            integralFBy = 0.0f;
            integralFBz = 0.0f;
        }

        // Apply proportional feedback
        gx += Kp * halfex;
        gy += Kp * halfey;
        gz += Kp * halfez;
    }

    // Integrate rate of change of quaternion
    gx *= 0.5f * dt;
    gy *= 0.5f * dt;
    gz *= 0.5f * dt;
    qa = q0;
    qb = q1;
    qc = q2;
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
}

void MahonyAHRS::getQuaternion(float& w, float& x, float& y, float& z) const {
    w = q0;
    x = q1;
    y = q2;
    z = q3;
}

void MahonyAHRS::reset() {
    q0 = 1.0f;
    q1 = q2 = q3 = 0.0f;
    integralFBx = integralFBy = integralFBz = 0.0f;
}

void MahonyAHRS::setGains(float kp, float ki) {
    Kp = kp;
    Ki = ki;
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
