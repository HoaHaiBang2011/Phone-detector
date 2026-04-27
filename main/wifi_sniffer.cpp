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
    uint8_t* payload = pkt->payload;
    
    // Dùng biến static để tránh malloc trong ISR (Cực kỳ quan trọng!)
    static SnifferPacket_t packet_data; 
    
    // Xác định vị trí MAC tùy theo loại gói tin
    // Thường MAC nguồn nằm ở offset 10 cho Data và offset 10 cho Management
    uint8_t* mac_ptr = payload + 10; 

    // Copy dữ liệu vào struct tạm
    memcpy(packet_data.mac, mac_ptr, 6);
    packet_data.rssi = pkt->rx_ctrl.rssi;
    packet_data.channel = pkt->rx_ctrl.channel;

    // Đẩy vào Queue (Dùng bản FromISR để không làm treo hệ thống)
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xQueueSendFromISR(sniffer_queue, &packet_data, &xHigherPriorityTaskWoken) == pdTRUE) {
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}
