#ifndef DEVICE_TRACKER_H
#define DEVICE_TRACKER_H

#include <Arduino.h>
#include "config.h"
#include "oui_database.h"

class DeviceTracker {
public:
    static void init();
    static void addDevice(const uint8_t* mac, int8_t rssi, uint8_t channel);
    static WiFiDevice_t* findDevice(const uint8_t* mac);
    static WiFiDevice_t* getDeviceList(); // Sửa lại thành con trỏ đơn
    static uint16_t getDeviceCount();
    static void sortDevices();
    static void clearDevices();
    
private:
    static WiFiDevice_t* devices;
    static uint16_t device_count;
    static bool isPrivateMac(const uint8_t* mac);
};

#endif
