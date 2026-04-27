#include "buzzer_controller.h"

int8_t   BuzzerController::_target_rssi = -100;
uint32_t BuzzerController::_last_beep_ms = 0;
bool     BuzzerController::_is_beeping = false;
bool     BuzzerController::_enabled = false;

void BuzzerController::init() {
    // Arduino ESP32 LEDC setup
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0); // Tắt lúc đầu
    _enabled = true;
}

void BuzzerController::setTargetRSSI(int8_t rssi) {
    _target_rssi = rssi;
}

void BuzzerController::update() {
    if (!_enabled || _target_rssi <= RSSI_FAR) {
        stop();
        return;
    }

    uint32_t now = millis();
    
    // Tính toán khoảng cách nghỉ giữa 2 tiếng bíp (ms)
    // Sóng càng mạnh (-30), interval càng ngắn (50ms) -> bíp liên tục
    // Sóng càng yếu (-90), interval càng dài (1000ms) -> bíp thưa
    long interval = map(_target_rssi, RSSI_FAR, RSSI_NEAR, 1000, 50);
    interval = constrain(interval, 50, 1000);

    // Thời gian của một tiếng bíp là 40ms (ngắn và gọn)
    uint16_t beep_duration = 40;

    if (!_is_beeping && (now - _last_beep_ms >= (uint32_t)interval)) {
        // Bắt đầu tiếng bíp
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQ);
        // Chỉnh độ sáng/âm lượng qua Duty Cycle (đã có trở hạn dòng nên để 50% là vừa)
        ledcWrite(BUZZER_CHANNEL, 128); 
        _last_beep_ms = now;
        _is_beeping = true;
    } 
    else if (_is_beeping && (now - _last_beep_ms >= beep_duration)) {
        // Kết thúc tiếng bíp
        ledcWrite(BUZZER_CHANNEL, 0);
        _is_beeping = false;
    }
}

void BuzzerController::stop() {
    ledcWrite(BUZZER_CHANNEL, 0);
    _is_beeping = false;
    _target_rssi = -100;
}
