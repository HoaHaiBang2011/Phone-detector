#ifndef WIFI_SNIFFER_H
#define WIFI_SNIFFER_H

#include <stdint.h>
#include "config.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Cấu trúc gói tin gọn nhẹ để truyền qua Queue
typedef struct {
    uint8_t mac[6];
    int8_t rssi;
    uint8_t channel;
} SnifferPacket_t;

class WiFiSniffer {
public:
    static void init();
    static void startSniffing();
    static void stopSniffing();
    static void setChannel(uint8_t channel);
    
private:
    // Callback xử lý gói tin hóng hớt được
    static void promiscuousRxCallback(void* buf, wifi_promiscuous_pkt_type_t type);
};

#endif
