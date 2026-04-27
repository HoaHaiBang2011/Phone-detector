#include "device_tracker.h"

WiFiDevice_t* DeviceTracker::devices = NULL;
uint16_t DeviceTracker::device_count = 0;

void DeviceTracker::init() {
    // Cấp phát vùng nhớ trên PSRAM cho 200 thiết bị
    devices = (WiFiDevice_t*) ps_malloc(sizeof(WiFiDevice_t) * MAX_DEVICES);
    if (!devices) {
        Serial.println("Loi: Khong the cap phat PSRAM!");
        return;
    }
    memset(devices, 0, sizeof(WiFiDevice_t) * MAX_DEVICES);
    device_count = 0;
}

bool DeviceTracker::isPrivateMac(const uint8_t* mac) {
    // Kiểm tra bit thứ 2 của byte đầu tiên (Locally Administered Bit)
    return (mac[0] & 0x02);
}

void DeviceTracker::addDevice(const uint8_t* mac, int8_t rssi, uint8_t channel) {
    if (!mac || !devices) return;
    
    WiFiDevice_t* dev = findDevice(mac);
    
    if (dev) {
        // Cập nhật thiết bị cũ
        dev->rssi = rssi;
        dev->last_seen = millis();
        
        // Tính trung bình trượt cho RSSI (Moving Average)
        dev->rssi_history[dev->rssi_idx] = rssi;
        dev->rssi_idx = (dev->rssi_idx + 1) % RSSI_SAMPLES;
        
        int16_t sum = 0;
        for(int i=0; i<RSSI_SAMPLES; i++) sum += dev->rssi_history[i];
        dev->rssi_sum = sum / RSSI_SAMPLES; 
        return;
    }
    
    // Thêm thiết bị mới nếu chưa đầy bộ nhớ
    if (device_count < MAX_DEVICES) {
        WiFiDevice_t* new_dev = &devices[device_count];
        memcpy(new_dev->mac, mac, 6);
        new_dev->rssi = rssi;
        new_dev->channel = channel;
        new_dev->last_seen = millis();
        new_dev->is_private = isPrivateMac(mac);
        
        if (new_dev->is_private) {
            strncpy(new_dev->oui_name, "An Danh [P]", 15);
        } else {
            const char* mfg = OUIDatabase::lookupManufacturer(mac);
            strncpy(new_device->oui_name, mfg, 15);
        }

        // Khởi tạo lịch sử RSSI
        for(int i=0; i<RSSI_SAMPLES; i++) new_dev->rssi_history[i] = rssi;
        new_dev->rssi_sum = rssi;
        new_dev->rssi_idx = 0;

        device_count++;
    }
}

WiFiDevice_t* DeviceTracker::findDevice(const uint8_t* mac) {
    for (uint16_t i = 0; i < device_count; i++) {
        if (memcmp(devices[i].mac, mac, 6) == 0) return &devices[i];
    }
    return NULL;
}

WiFiDevice_t* DeviceTracker::getDeviceList() {
    return devices;
}

uint16_t DeviceTracker::getDeviceCount() {
    return device_count;
}

void DeviceTracker::sortDevices() {
    // Sắp xếp nổi bọt (Bubble Sort) dựa trên RSSI trung bình
    for (uint16_t i = 0; i < device_count - 1; i++) {
        for (uint16_t j = i + 1; j < device_count; j++) {
            if (devices[j].rssi_sum > devices[i].rssi_sum) {
                WiFiDevice_t temp = devices[i];
                devices[i] = devices[j];
                devices[j] = temp;
            }
        }
    }
}

void DeviceTracker::clearDevices() {
    device_count = 0;
    if(devices) memset(devices, 0, sizeof(WiFiDevice_t) * MAX_DEVICES);
}
