#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include <stdint.h>
#include "config.h"

class SignalProcessor {
public:
    // Cập nhật RSSI và tính toán lại giá trị trung bình ngay lập tức
    static void updateRSSI(WiFiDevice_t* device, int8_t new_rssi);
    
    // Lấy RSSI trung bình (đã được tính sẵn, không tốn CPU để tính lại)
    static int8_t getAverageRSSI(const WiFiDevice_t* device);
    
    static uint8_t rssiToPercentage(int8_t rssi);
    static uint16_t rssiToBuzzerFreq(int8_t rssi);
    static uint8_t rssiToDutyCycle(int8_t rssi);
};

#endif
