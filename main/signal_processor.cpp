#include "signal_processor.h"
#include <Arduino.h> // Để dùng hàm map()

void SignalProcessor::updateRSSI(WiFiDevice_t* device, int8_t new_rssi) {
    if (!device) return;

    // Running Sum logic: Trừ mẫu cũ, cộng mẫu mới
    device->rssi_sum -= device->rssi_history[device->rssi_idx];
    device->rssi_history[device->rssi_idx] = new_rssi;
    device->rssi_sum += new_rssi;
    
    // Cập nhật index vòng lặp
    device->rssi_idx = (device->rssi_idx + 1) % RSSI_SAMPLES;
    device->rssi = new_rssi;
}

int8_t SignalProcessor::getAverageRSSI(const WiFiDevice_t* device) {
    if (!device || RSSI_SAMPLES == 0) return -100;
    // Chỉ mất 1 phép chia thay vì vòng lặp 32 lần
    return (int8_t)(device->rssi_sum / RSSI_SAMPLES);
}

uint8_t SignalProcessor::rssiToPercentage(int8_t rssi) {
    if (rssi >= RSSI_NEAR) return 100;
    if (rssi <= RSSI_FAR) return 0;
    
    // Ép dải RSSI thành 0-100%
    return (uint8_t)(((rssi - RSSI_FAR) * 100) / (RSSI_NEAR - RSSI_FAR));
}

uint16_t SignalProcessor::rssiToBuzzerFreq(int8_t rssi) {
    uint8_t pct = rssiToPercentage(rssi);
    // Tần số từ 400Hz (xa) đến 2500Hz (rất gần)
    return 400 + (pct * 21); 
}

uint8_t SignalProcessor::rssiToDutyCycle(int8_t rssi) {
    uint8_t pct = rssiToPercentage(rssi);
    // Độ lớn âm thanh: Gần thì kêu to, xa kêu nhỏ (Max 50% để bảo vệ loa)
    return (pct > 0) ? map(pct, 0, 100, 5, 50) : 0;
}
