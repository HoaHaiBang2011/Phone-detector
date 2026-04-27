#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "config.h"

class DisplayManager {
public:
    static void init();
    static void showSplash();
    static void drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint16_t selected_idx);
    static void drawTracker(WiFiDevice_t* device);
    
private:
    static TFT_eSPI tft;
    static TFT_Sprite img; // "Tờ giấy nháp" để vẽ chống nháy
};

#endif
