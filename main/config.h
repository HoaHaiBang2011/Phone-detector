#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/* ===== MEMORY CONFIGURATION ===== */
#define PSRAM_ENABLED 1
#define MAX_DEVICES 200
#define DEVICE_BUFFER_SIZE (MAX_DEVICES * sizeof(WiFiDevice_t))

/* ===== DEVICE STRUCT - Tiết kiệm bộ nhớ ===== */
typedef struct {
    uint8_t mac[6];              // MAC Address (6 bytes)
    int8_t rssi;                 // RSSI dBm
    uint8_t channel;             // Wi-Fi Channel (1-13)
    uint32_t last_seen;          // Timestamp ms
    char oui_name[16];           // OUI Manufacturer (16 bytes max)
    char temp_name[32];          // Temporary Device Name (Apple 1, Samsung 2...)
    uint8_t frame_count;         // Số frame phát hiện
    int8_t rssi_history[32];     // Moving Average Buffer (32 samples)
    uint8_t rssi_idx;            // Index trong RSSI history
} WiFiDevice_t;

/* ===== STATE MACHINE ===== */
typedef enum {
    MODE_DISCOVERY = 0,
    MODE_TARGET_TRACKING = 1
} SystemMode_t;

/* ===== DISPLAY CONFIG ===== */
#define TFT_WIDTH 240
#define TFT_HEIGHT 135
#define TFT_SCL_PIN 20
#define TFT_SDA_PIN 21
#define TFT_RST_PIN 47
#define TFT_DC_PIN 18
#define TFT_CS_PIN 45

/* ===== BUTTON CONFIG ===== */
#define BTN_UP_PIN 40
#define BTN_DOWN_PIN 42
#define BTN_SELECT_PIN 41
#define BTN_DEBOUNCE_MS 50

/* ===== BUZZER CONFIG ===== */
#define BUZZER_PIN 10
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 1000
#define BUZZER_RESOLUTION 8

/* ===== RSSI RANGE ===== */
#define RSSI_NEAR (-30)          // Rất gần
#define RSSI_FAR (-90)           // Rất xa
#define RSSI_SAMPLES 32          // Moving Average samples

/* ===== QUEUE CONFIG ===== */
#define SNIFFER_QUEUE_SIZE 64
#define SNIFFER_TASK_STACK 4096
#define SNIFFER_TASK_PRIORITY 15

/* ===== WIFI SCAN ===== */
#define WIFI_SCAN_INTERVAL_MS 100
#define WIFI_CHANNEL_COUNT 13

#endif // CONFIG_H
