#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

class BuzzerController {
public:
    static void init();
    static void setTargetRSSI(int8_t rssi); // Nhận RSSI từ target
    static void update();                  // Gọi trong loop() để xử lý nhịp bíp
    static void stop();
    
private:
    static int8_t  _target_rssi;
    static uint32_t _last_beep_ms;
    static bool    _is_beeping;
    static bool    _enabled;
};

#endif
