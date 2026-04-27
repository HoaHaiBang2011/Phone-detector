#include "wifi_sniffer.h"
#include "esp_log.h"

static const char* TAG = "WiFiSniffer";
extern QueueHandle_t sniffer_queue; // Queue này phải được tạo ở main

void WiFiSniffer::init() {
    // Khởi tạo WiFi theo chuẩn ESP32-S3
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL)); // Chế độ tiết kiệm điện nhất để sniff
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi Sniffer initialized");
}

void WiFiSniffer::startSniffing() {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(promiscuousRxCallback);
    
    // Chỉ lọc các gói Management (để bắt Probe Request) và Data
    wifi_promiscuous_filter_t filter = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
    };
    esp_wifi_set_promiscuous_filter(&filter);
}

void WiFiSniffer::stopSniffing() {
    esp_wifi_set_promiscuous(false);
}

void WiFiSniffer::setChannel(uint8_t channel) {
    if (channel >= 1 && channel <= 14) {
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    }
}

void WiFiSniffer::promiscuousRxCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*) buf;
    
    // Tạo struct tĩnh trên Stack của ISR, không dùng ps_malloc
    SnifferPacket_t sniffer_pkt; 
    
    uint8_t* payload = pkt->payload;
    uint8_t* mac_ptr = payload + 10; // Offset chuẩn cho Management frames

    memcpy(sniffer_pkt.mac, mac_ptr, 6);
    sniffer_pkt.rssi = pkt->rx_ctrl.rssi;
    sniffer_pkt.channel = pkt->rx_ctrl.channel;

    // Đẩy TRỰC TIẾP struct vào queue (không đẩy con trỏ)
    xQueueSendFromISR(sniffer_queue, &sniffer_pkt, NULL);
}
