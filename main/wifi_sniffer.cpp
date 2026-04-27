#include "wifi_sniffer.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

static const char* TAG = "WiFiSniffer";
extern QueueHandle_t sniffer_queue;

void WiFiSniffer::init() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Wi-Fi initialized in AP mode");
}

void WiFiSniffer::startSniffing() {
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    esp_wifi_set_promiscuous_rx_cb(promiscuousRxCallback);
    ESP_LOGI(TAG, "Promiscuous mode started");
}

void WiFiSniffer::stopSniffing() {
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(false));
    ESP_LOGI(TAG, "Promiscuous mode stopped");
}

void WiFiSniffer::setChannel(uint8_t channel) {
    if (channel > 0 && channel <= WIFI_CHANNEL_COUNT) {
        ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
    }
}

void WiFiSniffer::setFilter(uint32_t filter_mask) {
    wifi_promiscuous_filter_t filter = {
        .filter_mask = filter_mask
    };
    esp_wifi_set_promiscuous_filter(&filter);
}

int8_t WiFiSniffer::getRSSIFromPayload(wifi_promiscuous_pkt_t* pkt) {
    if (!pkt) return 0;
    return pkt->rx_ctrl.rssi;
}

void WiFiSniffer::promiscuousRxCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*) buf;
    
    if (!pkt) return;
    
    SnifferPacket_t* sniffer_pkt = (SnifferPacket_t*) ps_malloc(sizeof(SnifferPacket_t));
    if (!sniffer_pkt) return;
    
    // Extract MAC address using pointer arithmetic
    uint8_t* payload = pkt->payload;
    uint8_t* mac_ptr = NULL;
    
    if (type == WIFI_PKT_MGMT) {
        mac_ptr = payload + 4;
    } else if (type == WIFI_PKT_DATA) {
        mac_ptr = payload + 10;
    } else {
        free(sniffer_pkt);
        return;
    }
    
    memcpy(sniffer_pkt->mac, mac_ptr, 6);
    sniffer_pkt->rssi = pkt->rx_ctrl.rssi;
    sniffer_pkt->channel = pkt->rx_ctrl.channel;
    
    if (xQueueSendFromISR(sniffer_queue, &sniffer_pkt, NULL) != pdTRUE) {
        free(sniffer_pkt);
    }
}
