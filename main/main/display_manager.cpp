#include "display_manager.h"
#include "esp_log.h"

static const char* TAG = "DisplayManager";

void DisplayManager::init() {
    ESP_LOGI(TAG, "Display manager initialized (TFT 1.54 inch)");
}

void DisplayManager::clearDisplay() {
    ESP_LOGD(TAG, "Display cleared");
}

void DisplayManager::drawTitle(const char* title) {
    if (!title) return;
    ESP_LOGD(TAG, "Title: %s", title);
}

void DisplayManager::drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint8_t scroll_offset) {
    if (!devices || count == 0) {
        ESP_LOGD(TAG, "No devices to display");
        return;
    }
    
    uint8_t visible_items = (TFT_HEIGHT - 25) / 16;
    uint8_t start_idx = scroll_offset;
    uint8_t end_idx = (start_idx + visible_items < count) ? start_idx + visible_items : count;
    
    ESP_LOGD(TAG, "Drawing device list: %u devices, scroll offset %u", count, scroll_offset);
    
    for (uint8_t i = start_idx; i < end_idx; i++) {
        ESP_LOGD(TAG, "Device %u: %s - RSSI: %d dBm", i, devices[i].temp_name, devices[i].rssi);
    }
}

void DisplayManager::drawRSSIBar(uint8_t percentage, int8_t rssi_dbm) {
    ESP_LOGD(TAG, "RSSI Bar: %u%% (%d dBm)", percentage, rssi_dbm);
}

void DisplayManager::drawMACAddress(const uint8_t* mac) {
    if (!mac) return;
    ESP_LOGD(TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X", 
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
