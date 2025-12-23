#include "TwistCalculator.h"
#include <cmath>

TwistCalculator::TwistCalculator()
    : hasPrev_(false)
    , prev_() {
    cumulativeTheta_[0] = cumulativeTheta_[1] = cumulativeTheta_[2] = 0.0f;
}

void TwistCalculator::reset() {
    hasPrev_ = false;
    cumulativeTheta_[0] = cumulativeTheta_[1] = cumulativeTheta_[2] = 0.0f;
}

void TwistCalculator::reset(const Quaternion& initial) {
    hasPrev_ = true;
    prev_ = normalize(initial);
    cumulativeTheta_[0] = cumulativeTheta_[1] = cumulativeTheta_[2] = 0.0f;
}

TwistData TwistCalculator::step(const Quaternion& current) {
    Quaternion q = normalize(current);

    if (!hasPrev_) {
        reset(q);
        return getResult();
    }

    // Ensure quaternion continuity (flip sign if needed)
    if (dot(prev_, q) < 0.0f) {
        q.w = -q.w;
        q.x = -q.x;
        q.y = -q.y;
        q.z = -q.z;
    }

    // Compute local delta rotation: dq = inv(prev) * current
    Quaternion invPrev = inverse(prev_);
    Quaternion dq = multiply(invPrev, q);
    dq = normalize(dq);

    // Extract twist angles for each axis
    float dtx = extractTwistAngle(dq, 0);  // X axis
    float dty = extractTwistAngle(dq, 1);  // Y axis
    float dtz = extractTwistAngle(dq, 2);  // Z axis

    // Integrate
    cumulativeTheta_[0] += dtx;
    cumulativeTheta_[1] += dty;
    cumulativeTheta_[2] += dtz;

    prev_ = q;

    return getResult();
}

TwistData TwistCalculator::getResult() const {
    TwistData result;

    for (int i = 0; i < 3; i++) {
        result.theta[i] = cumulativeTheta_[i];
        result.angle[i] = repeatPositive(cumulativeTheta_[i], TAU);
        result.turns[i] = floorDiv(cumulativeTheta_[i], TAU);
    }

    return result;
}

Quaternion TwistCalculator::normalize(const Quaternion& q) {
    float mag = std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    if (mag > 1e-8f) {
        float inv = 1.0f / mag;
        return Quaternion(q.w * inv, q.x * inv, q.y * inv, q.z * inv);
    }
    return Quaternion();  // Identity
}

Quaternion TwistCalculator::inverse(const Quaternion& q) {
    // For unit quaternion, inverse = conjugate
    return Quaternion(q.w, -q.x, -q.y, -q.z);
}

Quaternion TwistCalculator::multiply(const Quaternion& a, const Quaternion& b) {
    return Quaternion(
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
    );
}

float TwistCalculator::dot(const Quaternion& a, const Quaternion& b) {
    return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

float TwistCalculator::extractTwistAngle(const Quaternion& dq, int axis) {
    // dq = (w, x, y, z)
    // Twist angle about axis = 2 * atan2(dot(xyz, axis_unit), w)

    float s;
    switch (axis) {
        case 0: s = dq.x; break;  // X axis (1, 0, 0)
        case 1: s = dq.y; break;  // Y axis (0, 1, 0)
        case 2: s = dq.z; break;  // Z axis (0, 0, 1)
        default: s = 0.0f;
    }

    return 2.0f * std::atan2(s, dq.w);
}

float TwistCalculator::repeatPositive(float x, float length) {
    // Returns x mod length in range [0, length)
    float r = x - std::floor(x / length) * length;
    return r;
}

int TwistCalculator::floorDiv(float x, float length) {
    return static_cast<int>(std::floor(x / length));
}
