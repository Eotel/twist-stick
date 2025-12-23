#pragma once

// IMU settings
constexpr float IMU_SAMPLE_RATE = 100.0f;  // Hz
constexpr uint32_t IMU_UPDATE_INTERVAL_US = 1000000 / (uint32_t)IMU_SAMPLE_RATE;

// Mahony filter gains
constexpr float MAHONY_KP = 0.5f;  // Proportional gain
constexpr float MAHONY_KI = 0.0f;  // Integral gain (set to 0 for 6DoF)

// OSC settings
constexpr uint16_t OSC_SEND_PORT = 9000;
constexpr uint16_t OSC_RECV_PORT = 9001;
constexpr const char* OSC_ADDRESS = "/twist-stick";
constexpr const char* OSC_QUATERNION_ADDRESS = "/twist-stick/quaternion";
constexpr const char* OSC_RESET_ADDRESS = "/twist-stick/reset";

// WiFi settings
constexpr const char* AP_SSID = "TwistStick-Config";
constexpr const char* AP_PASSWORD = "";  // Open network for config
constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 10000;

// Config file
constexpr const char* CONFIG_FILE = "/config.json";

// Display update rate
constexpr uint32_t DISPLAY_UPDATE_INTERVAL_MS = 100;

// Display power saving
constexpr uint32_t DISPLAY_TIMEOUT_MS = 10000;  // 10 seconds
