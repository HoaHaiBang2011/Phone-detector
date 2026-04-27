#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "config.h"
#include "wifi_sniffer.h"
#include "device_tracker.h"
#include "signal_processor.h"
#include "buzzer_controller.h"
#include "display_manager.h"
#include "oui_database.h"

static const char* TAG = "PhoneDetector";

QueueHandle_t sniffer_queue = NULL;
SystemMode_t system_mode = MODE_DISCOVERY;
WiFiDevice_t* selected_device = NULL;
uint8_t current_channel = 1;
uint8_t scroll_offset = 0;
uint32_t last_button_press[3] = {0, 0, 0};

void snifferTask(void* pvParameters) {
    SnifferPacket_t* pkt = NULL; // Đây là con trỏ để nhận địa chỉ từ Queue
    ESP_LOGI(TAG, "Sniffer task started");
    
    while (1) {
        // Nhận địa chỉ vùng nhớ được gửi từ WiFiSniffer
        if (xQueueReceive(sniffer_queue, &pkt, portMAX_DELAY) == pdTRUE) {
            if (pkt == NULL) continue;
            
            switch (system_mode) {
                case MODE_DISCOVERY:
                    DeviceTracker::addDevice(pkt->mac, pkt->rssi, pkt->channel);
                    break;
                    
                case MODE_TARGET_TRACKING:
                    if (selected_device && memcmp(pkt->mac, selected_device->mac, 6) == 0) {
                        SignalProcessor::updateRSSI(selected_device, pkt->rssi);
                        // Cập nhật âm thanh dựa trên RSSI trung bình đã qua xử lý
                        BuzzerController::play(SignalProcessor::getAverageRSSI(selected_device));
                    }
                    break;
            }
            
            // Giải phóng đúng vùng nhớ đã malloc bên wifi_sniffer.cpp
            free(pkt); 
        }
    }
}

void displayTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    ESP_LOGI(TAG, "Display task started");
    
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
        
        switch (system_mode) {
            case MODE_DISCOVERY: {
                uint16_t device_count = 0;
                WiFiDevice_t** device_list = DeviceTracker::getDeviceList(&device_count);
                
                if (device_list && device_count > 0) {
                    DisplayManager::clearDisplay();
                    DisplayManager::drawTitle("Wi-Fi Discovery");
                    DisplayManager::drawDeviceList(*device_list, device_count, scroll_offset);
                    ESP_LOGD(TAG, "Discovery mode: %u devices found", device_count);
                } else {
                    DisplayManager::clearDisplay();
                    DisplayManager::drawTitle("Scanning...");
                }
                break;
            }
            
            case MODE_TARGET_TRACKING: {
                if (selected_device) {
                    DisplayManager::clearDisplay();
                    DisplayManager::drawTitle(selected_device->temp_name);
                    
                    int8_t avg_rssi = SignalProcessor::getAverageRSSI(selected_device);
                    uint8_t percentage = SignalProcessor::rssiToPercentage(avg_rssi);
                    
                    DisplayManager::drawRSSIBar(percentage, avg_rssi);
                    DisplayManager::drawMACAddress(selected_device->mac);
                    
                    ESP_LOGD(TAG, "Tracking: %s - RSSI: %d dBm (%u%%)", 
                            selected_device->temp_name, avg_rssi, percentage);
                }
                break;
            }
        }
    }
}

void buttonTask(void* pvParameters) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BTN_UP_PIN) | (1ULL << BTN_DOWN_PIN) | (1ULL << BTN_SELECT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "Button task started");
    
    while (1) {
        uint32_t now = esp_timer_get_time() / 1000;
        
        // Button UP (Pin 40)
        if (gpio_get_level(BTN_UP_PIN) == 0) {
            if ((now - last_button_press[0]) > BTN_DEBOUNCE_MS) {
                last_button_press[0] = now;
                
                if (system_mode == MODE_DISCOVERY && scroll_offset > 0) {
                    scroll_offset--;
                    ESP_LOGI(TAG, "Scroll up: offset = %d", scroll_offset);
                }
            }
        }
        
        // Button DOWN (Pin 42)
        if (gpio_get_level(BTN_DOWN_PIN) == 0) {
            if ((now - last_button_press[1]) > BTN_DEBOUNCE_MS) {
                last_button_press[1] = now;
                
                if (system_mode == MODE_DISCOVERY) {
                    uint16_t device_count = 0;
                    DeviceTracker::getDeviceList(&device_count);
                    
                    if (scroll_offset < device_count - 1) {
                        scroll_offset++;
                        ESP_LOGI(TAG, "Scroll down: offset = %d", scroll_offset);
                    }
                }
            }
        }
        
        // Button SELECT (Pin 41)
        if (gpio_get_level(BTN_SELECT_PIN) == 0) {
            if ((now - last_button_press[2]) > BTN_DEBOUNCE_MS) {
                last_button_press[2] = now;
                
                if (system_mode == MODE_DISCOVERY) {
                    uint16_t device_count = 0;
                    WiFiDevice_t** device_list = DeviceTracker::getDeviceList(&device_count);
                    
                    if (scroll_offset < device_count && device_list) {
                        selected_device = &(*device_list)[scroll_offset];
                        system_mode = MODE_TARGET_TRACKING;
                        
                        ESP_LOGI(TAG, "Entering TRACKING mode for: %s", selected_device->temp_name);
                        
                        WiFiSniffer::stopSniffing();
                        vTaskDelay(pdMS_TO_TICKS(100));
                        WiFiSniffer::setChannel(selected_device->channel);
                        WiFiSniffer::startSniffing();
                        
                        BuzzerController::init();
                    }
                    
                } else if (system_mode == MODE_TARGET_TRACKING) {
                    system_mode = MODE_DISCOVERY;
                    
                    ESP_LOGI(TAG, "Exiting TRACKING mode, back to DISCOVERY");
                    
                    BuzzerController::stop();
                    selected_device = NULL;
                    
                    WiFiSniffer::startSniffing();
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void channelScanTask(void* pvParameters) {
    while (1) {
        // Chỉ quét kênh khi đang ở chế độ Discovery
        if (system_mode == MODE_DISCOVERY) {
            WiFiSniffer::setChannel(current_channel);
            current_channel = (current_channel % WIFI_CHANNEL_COUNT) + 1;
            
            if (current_channel == 1) {
                DeviceTracker::sortDevices();
            }
        }
        // Để 200-300ms để bắt được nhiều gói tin hơn trên mỗi kênh
        vTaskDelay(pdMS_TO_TICKS(WIFI_SCAN_INTERVAL_MS)); 
    }
}
extern "C" void app_main(void) {
    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   Phone Detector - WiFi Sniffer v1.0   ║");
    ESP_LOGI(TAG, "║   ESP32-S3 N16R8 Device Tracker        ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS flash needs erasing");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    sniffer_queue = xQueueCreate(SNIFFER_QUEUE_SIZE, sizeof(SnifferPacket_t*));
    if (!sniffer_queue) {
        ESP_LOGE(TAG, "Failed to create sniffer queue");
        return;
    }
    ESP_LOGI(TAG, "Sniffer queue created (size: %d)", SNIFFER_QUEUE_SIZE);
    
    DeviceTracker::init();
    WiFiSniffer::init();
    BuzzerController::init();
    DisplayManager::init();
    
    wifi_promiscuous_filter_t filter = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
    };
    esp_wifi_set_promiscuous_filter(&filter);
    ESP_LOGI(TAG, "Wi-Fi filter configured (MGMT + DATA frames)");
    
    WiFiSniffer::startSniffing();
    ESP_LOGI(TAG, "Wi-Fi promiscuous mode activated");
    
    xTaskCreatePinnedToCore(snifferTask, "Sniffer", SNIFFER_TASK_STACK, NULL, 
                           SNIFFER_TASK_PRIORITY, NULL, 0);
    ESP_LOGI(TAG, "✓ Sniffer task created on Core 0");
    
    xTaskCreatePinnedToCore(displayTask, "Display", 2048, NULL, 10, NULL, 1);
    ESP_LOGI(TAG, "✓ Display task created on Core 1");
    
    xTaskCreatePinnedToCore(buttonTask, "Button", 2048, NULL, 10, NULL, 1);
    ESP_LOGI(TAG, "✓ Button task created on Core 1");
    
    xTaskCreatePinnedToCore(channelScanTask, "ChannelScan", 2048, NULL, 12, NULL, 0);
    ESP_LOGI(TAG, "✓ Channel scan task created on Core 0");
    
    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   System Ready - All Tasks Created     ║");
    ESP_LOGI(TAG, "║   MODE: DISCOVERY                      ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
}
