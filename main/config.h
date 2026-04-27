#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h> // Thay stdint bằng Arduino để dùng được các kiểu dữ liệu board

/* ===== MEMORY CONFIGURATION ===== */
#define PSRAM_ENABLED 1
#define MAX_DEVICES 200 // Với 8MB PSRAM, bồ có thể tăng lên 1000 nếu muốn

/* ===== DEVICE STRUCT - Tối ưu cho PSRAM ===== */
typedef struct {
    uint8_t mac[6];              // MAC Address
    int8_t rssi;                 // RSSI hiện tại
    uint8_t channel;             // Kênh bắt được
    uint32_t last_seen;          // Thời điểm cuối nhìn thấy (ms)
    char oui_name[16];           // Tên NSX (Apple, Samsung...)
    bool is_private;             // Đánh dấu nếu là MAC ảo
    int16_t rssi_sum;            // Dùng để tính trung bình trượt nhanh hơn
    int8_t rssi_history[16];     // Giảm xuống 16 mẫu để phản hồi nhanh hơn
    uint8_t rssi_idx;
} WiFiDevice_t;

/* ===== STATE MACHINE ===== */
typedef enum {
    MODE_DISCOVERY = 0,
    MODE_TARGET_TRACKING = 1
} SystemMode_t;

/* ===== DISPLAY CONFIG (ST7789 240x240) ===== */
#define TFT_WIDTH 240
#define TFT_HEIGHT 240  // Đã sửa từ 135 thành 240
#define TFT_SCL_PIN 20
#define TFT_SDA_PIN 21
#define TFT_RST_PIN 47
#define TFT_DC_PIN 18
#define TFT_CS_PIN 45

/* ===== BUTTON CONFIG (Giữ nguyên Pin đã test) ===== */
#define BTN_UP_PIN 40
#define BTN_DOWN_PIN 42
#define BTN_SELECT_PIN 41
#define BTN_DEBOUNCE_MS 50 // Thời gian chờ ổn định nút nhấn (ms)

/* ===== BUZZER CONFIG (Dùng LEDC PWM) ===== */
#define BUZZER_PIN 10
#define BUZZER_CHANNEL 0
#define BUZZER_FREQ 2000     // Tần số 2kHz nghe thanh và rõ hơn
#define BUZZER_RESOLUTION 8  // 8-bit (0-255) dùng để chỉnh âm lượng (Duty Cycle)

/* ===== RSSI RANGE ===== */
#define RSSI_NEAR (-30)      // Vạch Max
#define RSSI_FAR (-90)       // Vạch Min
#define RSSI_SAMPLES 16      // Số mẫu lấy trung bình

/* ===== WIFI SCAN ===== */
#define WIFI_CHANNEL_COUNT 13
#define CHANNEL_HOP_INTERVAL 150 // Thời gian ở lại mỗi kênh (ms)

#endif // CONFIG_H
