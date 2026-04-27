#ifndef WIFI_SNIFFER_H
#define WIFI_SNIFFER_H

#include <stdint.h>
#include "config.h"
#include "esp_wifi.h"

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
    static void setFilter(uint32_t filter_mask);
    
private:
    static void promiscuousRxCallback(void* buf, wifi_promiscuous_pkt_type_t type);
    static int8_t getRSSIFromPayload(wifi_promiscuous_pkt_t* pkt);
};

#endif // WIFI_SNIFFER_H
