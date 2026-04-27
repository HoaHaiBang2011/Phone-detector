#include "device_tracker.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

static const char* TAG = "DeviceTracker";

WiFiDevice_t* DeviceTracker::devices = NULL;
uint16_t DeviceTracker::device_count = 0;

void DeviceTracker::init() {
    devices = (WiFiDevice_t*) ps_malloc(DEVICE_BUFFER_SIZE);
    if (!devices) {
        ESP_LOGE(TAG, "Failed to allocate PSRAM for devices");
        return;
    }
    memset(devices, 0, DEVICE_BUFFER_SIZE);
    device_count = 0;
    ESP_LOGI(TAG, "Device tracker initialized (%.1f KB PSRAM)", DEVICE_BUFFER_SIZE / 1024.0);
}

void DeviceTracker::addDevice(const uint8_t* mac, int8_t rssi, uint8_t channel) {
    if (!mac || !devices) return;
    
    WiFiDevice_t* existing = findDevice(mac);
    if (existing) {
        existing->rssi = rssi;
        existing->last_seen = esp_timer_get_time() / 1000;
        existing->frame_count++;
        return;
    }
    
    if (device_count >= MAX_DEVICES) return;
    
    WiFiDevice_t* new_device = &devices[device_count];
    memcpy(new_device->mac, mac, 6);
    new_device->rssi = rssi;
    new_device->channel = channel;
    new_device->last_seen = esp_timer_get_time() / 1000;
    new_device->frame_count = 1;
    
    const char* mfg = OUIDatabase::lookupManufacturer(mac);
    strncpy(new_device->oui_name, mfg, sizeof(new_device->oui_name) - 1);
    
    generateTempName(new_device);
    
    memset(new_device->rssi_history, rssi, 32);
    new_device->rssi_idx = 0;
    
    device_count++;
    ESP_LOGD(TAG, "New device: %s (%02X:%02X:%02X)", new_device->temp_name, 
             mac[3], mac[4], mac[5]);
}

WiFiDevice_t* DeviceTracker::findDevice(const uint8_t* mac) {
    if (!mac || !devices) return NULL;
    
    for (uint16_t i = 0; i < device_count; i++) {
        if (memcmp(devices[i].mac, mac, 6) == 0) {
            return &devices[i];
        }
    }
    return NULL;
}

void DeviceTracker::generateTempName(WiFiDevice_t* device) {
    if (!device) return;
    
    uint8_t count = getManufacturerCount(device->oui_name);
    snprintf(device->temp_name, sizeof(device->temp_name), 
             "%s %u", device->oui_name, count);
}

uint8_t DeviceTracker::getManufacturerCount(const char* manufacturer) {
    uint8_t count = 0;
    for (uint16_t i = 0; i < device_count; i++) {
        if (strcmp(devices[i].oui_name, manufacturer) == 0) {
            count++;
        }
    }
    return count;
}

WiFiDevice_t** DeviceTracker::getDeviceList(uint16_t* count) {
    if (count) *count = device_count;
    return &devices;
}

void DeviceTracker::sortDevices() {
    for (uint16_t i = 0; i < device_count - 1; i++) {
        for (uint16_t j = i + 1; j < device_count; j++) {
            if (devices[j].rssi > devices[i].rssi) {
                WiFiDevice_t temp = devices[i];
                devices[i] = devices[j];
                devices[j] = temp;
            }
        }
    }
}

void DeviceTracker::clearDevices() {
    memset(devices, 0, DEVICE_BUFFER_SIZE);
    device_count = 0;
}
