#include "DisplayManager.h"
#include "../config.h"

DisplayManager::DisplayManager()
    : screen_(Screen::BOOT)
    , lastUpdate_(0)
    , lastActivity_(0)
    , needsRedraw_(true)
    , sleeping_(false) {
}

void DisplayManager::begin() {
    M5.Display.setRotation(1);  // Landscape mode for M5StickC Plus
    M5.Display.setTextSize(1);
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setBrightness(80);
    lastActivity_ = millis();
    sleeping_ = false;
}

void DisplayManager::update() {
    uint32_t now = millis();
    if (now - lastUpdate_ < DISPLAY_UPDATE_INTERVAL_MS && !needsRedraw_) {
        return;
    }
    lastUpdate_ = now;
    needsRedraw_ = false;

    // Check for timeout (only in RUNNING screen)
    if (screen_ == Screen::RUNNING) {
        checkTimeout();
    }
}

void DisplayManager::setScreen(Screen screen) {
    if (screen_ != screen) {
        screen_ = screen;
        needsRedraw_ = true;
        clear();
    }
}

void DisplayManager::clear() {
    M5.Display.fillScreen(TFT_BLACK);
}

void DisplayManager::drawHeader(const char* title) {
    M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(5, 5);
    M5.Display.print(title);
    M5.Display.drawLine(0, 25, M5.Display.width(), 25, TFT_CYAN);
}

void DisplayManager::showBoot() {
    setScreen(Screen::BOOT);
    clear();
    drawHeader("TwistStick");

    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 40);
    M5.Display.print("Starting...");
}

void DisplayManager::showCalibrating(int progress) {
    if (screen_ != Screen::CALIBRATING) {
        setScreen(Screen::CALIBRATING);
        clear();
        drawHeader("Calibrating");
    }

    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 40);
    M5.Display.print("Keep device still");

    // Progress bar
    int barWidth = M5.Display.width() - 20;
    int barX = 10;
    int barY = 60;
    int barHeight = 10;

    M5.Display.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);
    M5.Display.fillRect(barX + 1, barY + 1,
        (barWidth - 2) * progress / 100, barHeight - 2, TFT_GREEN);
}

void DisplayManager::showConnecting(const String& ssid) {
    setScreen(Screen::CONNECTING);
    clear();
    drawHeader("Connecting");

    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 40);
    M5.Display.print("SSID: ");
    M5.Display.print(ssid);
}

void DisplayManager::showConfigMode(const String& apIp) {
    setScreen(Screen::CONFIG_MODE);
    clear();
    drawHeader("Config Mode");

    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 35);
    M5.Display.print("Connect to WiFi:");
    M5.Display.setCursor(5, 50);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.print(AP_SSID);

    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Display.setCursor(5, 70);
    M5.Display.print("Then open:");
    M5.Display.setCursor(5, 85);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.print("http://");
    M5.Display.print(apIp);
}

void DisplayManager::showRunning(const String& ip, const TwistData& data) {
    if (screen_ != Screen::RUNNING) {
        setScreen(Screen::RUNNING);
        clear();
        drawHeader("Running");
    }

    // Skip drawing if sleeping to save power
    if (sleeping_) {
        return;
    }

    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.setTextSize(1);

    // IP address
    M5.Display.setCursor(5, 30);
    M5.Display.print("IP: ");
    M5.Display.print(ip);
    M5.Display.print("   ");  // Clear trailing chars

    // Twist data - show turns for each axis
    M5.Display.setCursor(5, 50);
    M5.Display.printf("X: %+4d  %.1f   ", data.turns[0], data.angle[0] * 180.0f / M_PI);

    M5.Display.setCursor(5, 65);
    M5.Display.printf("Y: %+4d  %.1f   ", data.turns[1], data.angle[1] * 180.0f / M_PI);

    M5.Display.setCursor(5, 80);
    M5.Display.printf("Z: %+4d  %.1f   ", data.turns[2], data.angle[2] * 180.0f / M_PI);
}

void DisplayManager::showError(const String& message) {
    setScreen(Screen::ERROR);
    clear();
    drawHeader("Error");

    M5.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(5, 40);
    M5.Display.print(message);
}

void DisplayManager::wake() {
    if (sleeping_) {
        sleeping_ = false;
        M5.Display.wakeup();
        M5.Display.waitDisplay();
        M5.Display.setBrightness(80);
        // Force full redraw
        clear();
        if (screen_ == Screen::RUNNING) {
            drawHeader("Running");
        }
        needsRedraw_ = true;
    }
    lastActivity_ = millis();
}

void DisplayManager::sleep() {
    if (!sleeping_) {
        sleeping_ = true;
        M5.Display.sleep();
        M5.Display.setBrightness(0);
        M5.Display.waitDisplay();
    }
}

void DisplayManager::resetTimeout() {
    lastActivity_ = millis();
}

void DisplayManager::checkTimeout() {
    if (!sleeping_ && (millis() - lastActivity_ >= DISPLAY_TIMEOUT_MS)) {
        sleep();
    }
}
