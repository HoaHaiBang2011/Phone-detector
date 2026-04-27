// display_manager.cpp
#include "display_manager.h"
#include "esp_log.h"
#include "driver/i2c.h"

static const char* TAG = "DisplayManager";

// TFT Display driver (simplified SPI/I2C interface)
class TFTDriver {
public:
    static void init() {
        // Initialize SPI/I2C for TFT display
        // Configure pins: SCL-20, SDA-21, RST-47, DC-18, CS-45
    }
    
    static void writeCommand(uint8_t cmd) {
        // Write command to display
    }
    
    static void writeData(uint8_t* data, uint16_t len) {
        // Write data to display
    }
    
    static void setWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
        // Set display window
    }
};

void DisplayManager::init() {
    TFTDriver::init();
    clearDisplay();
    ESP_LOGI(TAG, "Display initialized");
}

void DisplayManager::clearDisplay() {
    drawRectangle(0, 0, TFT_WIDTH, TFT_HEIGHT, COLOR_BLACK);
}

void DisplayManager::drawTitle(const char* title) {
    // Draw title bar at top with dark blue background
    drawRectangle(0, 0, TFT_WIDTH, 20, COLOR_DARK_BLUE);
    drawText(5, 5, title, COLOR_CYAN, 2);
}

void DisplayManager::drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint8_t scroll_offset) {
    if (!devices || count == 0) {
        drawText(10, 40, "No devices found", COLOR_WHITE, 1);
        return;
    }
    
    uint8_t visible_items = (TFT_HEIGHT - 25) / 16;  // ~7 items per screen
    uint8_t start_idx = scroll_offset;
    uint8_t end_idx = (start_idx + visible_items < count) ? start_idx + visible_items : count;
    
    for (uint8_t i = start_idx; i < end_idx; i++) {
        uint8_t display_row = i - start_idx;
        uint16_t y_pos = 25 + (display_row * 16);
        
        // Highlight selected item
        if (i == scroll_offset) {
            drawRectangle(0, y_pos, TFT_WIDTH, 15, COLOR_LIGHT_BLUE);
        }
        
        // Format: "Manufacturer - MAC(last 3 bytes) - RSSI"
        char buffer[40];
        snprintf(buffer, sizeof(buffer), "%s - %02X:%02X:%02X - %ddBm",
                devices[i].temp_name,
                devices[i].mac[3],
                devices[i].mac[4],
                devices[i].mac[5],
                devices[i].rssi);
        
        drawText(5, y_pos + 2, buffer, COLOR_CYAN, 1);
    }
    
    // Draw scroll indicator
    if (count > visible_items) {
        uint8_t scroll_bar_height = (TFT_HEIGHT - 25) * visible_items / count;
        uint8_t scroll_bar_y = 25 + (scroll_offset * (TFT_HEIGHT - 25)) / count;
        drawRectangle(TFT_WIDTH - 3, scroll_bar_y, 3, scroll_bar_height, COLOR_CYAN);
    }
}

void DisplayManager::drawRSSIBar(uint8_t percentage, int8_t rssi_dbm) {
    // Draw RSSI progress bar in center of screen
    uint16_t bar_width = 200;
    uint16_t bar_height = 30;
    uint16_t bar_x = (TFT_WIDTH - bar_width) / 2;
    uint16_t bar_y = 50;
    
    // Draw background (empty bar)
    drawRectangle(bar_x, bar_y, bar_width, bar_height, COLOR_DARK_BLUE);
    
    // Draw filled portion based on percentage
    uint16_t filled_width = (bar_width - 4) * percentage / 100;
    
    // Color gradient: Red (far) -> Yellow -> Green (near)
    uint16_t bar_color;
    if (percentage < 33) {
        bar_color = 0xF800;  // Red
    } else if (percentage < 66) {
        bar_color = 0xFFE0;  // Yellow
    } else {
        bar_color = COLOR_GREEN;  // Green
    }
    
    drawRectangle(bar_x + 2, bar_y + 2, filled_width, bar_height - 4, bar_color);
    
    // Draw percentage text
    char pct_text[16];
    snprintf(pct_text, sizeof(pct_text), "%d%%", percentage);
    drawText(bar_x + bar_width / 2 - 15, bar_y + 8, pct_text, COLOR_WHITE, 2);
    
    // Draw RSSI dBm text
    char rssi_text[16];
    snprintf(rssi_text, sizeof(rssi_text), "%d dBm", rssi_dbm);
    drawText(bar_x + bar_width / 2 - 20, bar_y + bar_height + 10, rssi_text, COLOR_CYAN, 1);
}

void DisplayManager::drawMACAddress(const uint8_t* mac) {
    if (!mac) return;
    
    char mac_str[20];
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    drawText(10, TFT_HEIGHT - 20, mac_str, COLOR_WHITE, 1);
}

void DisplayManager::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    // Platform-specific pixel drawing using TFT driver
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    // Implementation depends on TFT library (e.g., Adafruit_ST7789, LilyGO ST7789)
}

void DisplayManager::drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    // Draw filled rectangle
    for (uint16_t i = x; i < x + width; i++) {
        for (uint16_t j = y; j < y + height; j++) {
            drawPixel(i, j, color);
        }
    }
}

void DisplayManager::drawText(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size) {
    // Simple text rendering (font rendering depends on TFT library)
    // This is a placeholder - use tft.print() or custom font renderer
}
