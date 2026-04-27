#include "signal_processor.h"

void SignalProcessor::updateRSSI(WiFiDevice_t* device, int8_t new_rssi) {
    if (!device) return;
    
    device->rssi_history[device->rssi_idx] = new_rssi;
    device->rssi_idx = (device->rssi_idx + 1) % RSSI_SAMPLES;
    device->rssi = new_rssi;
}

int8_t SignalProcessor::getAverageRSSI(const WiFiDevice_t* device) {
    if (!device) return 0;
    
    int32_t sum = 0;
    for (uint8_t i = 0; i < RSSI_SAMPLES; i++) {
        sum += device->rssi_history[i];
    }
    return (int8_t)(sum / RSSI_SAMPLES);
}

uint8_t SignalProcessor::rssiToPercentage(int8_t rssi) {
    if (rssi >= RSSI_NEAR) return 100;
    if (rssi <= RSSI_FAR) return 0;
    
    int8_t range = RSSI_NEAR - RSSI_FAR;
    int8_t offset = rssi - RSSI_FAR;
    return (uint8_t)((offset * 100) / range);
}

uint16_t SignalProcessor::rssiToBuzzerFreq(int8_t rssi) {
    uint8_t percentage = rssiToPercentage(rssi);
    return 500 + (percentage * 15) / 10;
}

uint8_t SignalProcessor::rssiToDutyCycle(int8_t rssi) {
    uint8_t percentage = rssiToPercentage(rssi);
    return 80 - (percentage * 50) / 100;
}
