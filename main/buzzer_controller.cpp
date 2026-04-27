#include "buzzer_controller.h"
#include "driver/ledc.h"
#include "signal_processor.h"
#include "esp_log.h"

static const char* TAG = "BuzzerController";

uint16_t BuzzerController::current_freq = 1000;
uint8_t BuzzerController::current_duty = 0;

void BuzzerController::init() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = (ledc_timer_bit_t)BUZZER_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = BUZZER_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
    
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = (ledc_channel_t)BUZZER_CHANNEL,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BUZZER_PIN,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);
    
    ESP_LOGI(TAG, "Buzzer initialized on GPIO %d", BUZZER_PIN);
}

void BuzzerController::play(int8_t rssi) {
    uint16_t freq = SignalProcessor::rssiToBuzzerFreq(rssi);
    uint8_t duty = SignalProcessor::rssiToDutyCycle(rssi);
    
    setFrequency(freq);
    setDutyCycle(duty);
}

void BuzzerController::stop() {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)BUZZER_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)BUZZER_CHANNEL);
    ESP_LOGD(TAG, "Buzzer stopped");
}

void BuzzerController::setFrequency(uint16_t freq) {
    if (freq == current_freq) return;
    current_freq = freq;
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
}

void BuzzerController::setDutyCycle(uint8_t duty) {
    if (duty == current_duty) return;
    current_duty = duty;
    
    uint32_t duty_val = (duty * (1 << BUZZER_RESOLUTION)) / 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)BUZZER_CHANNEL, duty_val);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)BUZZER_CHANNEL);
}
