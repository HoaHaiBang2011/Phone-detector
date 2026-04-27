#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

#include <stdint.h>
#include "config.h"

class BuzzerController {
public:
    static void init();
    static void play(int8_t rssi);
    static void stop();
    static void setFrequency(uint16_t freq);
    static void setDutyCycle(uint8_t duty);
    
private:
    static uint16_t current_freq;
    static uint8_t current_duty;
};

#endif // BUZZER_CONTROLLER_H