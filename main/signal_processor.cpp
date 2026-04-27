#include "signal_processor.h"

void SignalProcessor::updateRSSI(WiFiDevice_t* device, int8_t new_rssi) {
    if (!device) return;

    // 1. Loại bỏ giá trị cũ nhất khỏi tổng
    device->rssi_sum -= device->rssi_history[device->rssi_idx];
    
    // 2. Ghi đè giá trị mới vào lịch sử
    device->rssi_history[device->rssi_idx] = new_rssi;
    
    // 3. Cộng giá trị mới vào tổng
    device->rssi_sum += new_rssi;
    
    // 4. Cập nhật chỉ số (Index) vòng lặp
    device->rssi_idx = (device->rssi_idx + 1) % RSSI_SAMPLES;
    
    // Lưu giá trị tức thời
    device->rssi = new_rssi;
}

int8_t SignalProcessor::getAverageRSSI(const WiFiDevice_t* device) {
    if (!device || RSSI_SAMPLES == 0) return -100;
    // Chỉ cần 1 phép chia, không cần vòng lặp for như file cũ của bồ
    return (int8_t)(device->rssi_sum / RSSI_SAMPLES);
}

uint8_t SignalProcessor::rssiToPercentage(int8_t rssi) {
    // Ép dải từ -100dBm đến -30dBm thành 0-100%
    if (rssi >= RSSI_NEAR) return 100;
    if (rssi <= RSSI_FAR) return 0;
    
    return (uint8_t)(((rssi - RSSI_FAR) * 100) / (RSSI_NEAR - RSSI_FAR));
}

uint16_t SignalProcessor::rssiToBuzzerFreq(int8_t rssi) {
    uint8_t pct = rssiToPercentage(rssi);
    // Càng gần (RSSI cao), tiếng bíp càng cao và gắt
    // 400Hz (trầm) -> 2500Hz (chói tai)
    return 400 + (pct * 21); 
}

uint8_t SignalProcessor::rssiToDutyCycle(int8_t rssi) {
    uint8_t pct = rssiToPercentage(rssi);
    // Nếu ở xa thì kêu nhỏ (duty thấp), ở gần kêu to (duty cao)
    // Tối đa 50% để bảo vệ loa buzzer của bồ
    return (pct > 0) ? map(pct, 0, 100, 5, 50) : 0;
}
