#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include <stdint.h>
#include "config.h"

class SignalProcessor {
public:
    static void updateRSSI(WiFiDevice_t* device, int8_t new_rssi);
    static int8_t getAverageRSSI(const WiFiDevice_t* device);
    static uint8_t rssiToPercentage(int8_t rssi);
    static uint16_t rssiToBuzzerFreq(int8_t rssi);
    static uint8_t rssiToDutyCycle(int8_t rssi);
    
private:
    static int8_t calculateMovingAverage(const int8_t* history, uint8_t size);
};

#endif // SIGNAL_PROCESSOR_H