#pragma once

#include "../imu/ImuReader.h"

struct TwistData {
    // Normalized angles [0, 2*PI)
    float angle[3];
    // Rotation counts (can be negative)
    int32_t turns[3];
    // Cumulative angles (unwrapped)
    float theta[3];
};

class TwistCalculator {
public:
    TwistCalculator();

    // Reset cumulative angles to zero
    void reset();
    void reset(const Quaternion& initial);

    // Process new quaternion and update twist angles
    TwistData step(const Quaternion& current);

    // Get current result without processing new data
    TwistData getResult() const;

private:
    static constexpr float TAU = 2.0f * M_PI;

    bool hasPrev_;
    Quaternion prev_;
    float cumulativeTheta_[3];

    // Quaternion operations
    static Quaternion normalize(const Quaternion& q);
    static Quaternion inverse(const Quaternion& q);
    static Quaternion multiply(const Quaternion& a, const Quaternion& b);
    static float dot(const Quaternion& a, const Quaternion& b);

    // Extract twist angle about a specific axis from a delta quaternion
    // axis: 0=X (right), 1=Y (up), 2=Z (forward)
    static float extractTwistAngle(const Quaternion& dq, int axis);

    // Math utilities
    static float repeatPositive(float x, float length);
    static int floorDiv(float x, float length);
};
