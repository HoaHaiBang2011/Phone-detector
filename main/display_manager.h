#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <stdint.h>
#include "config.h"

class DisplayManager {
public:
    static void init();
    static void clearDisplay();
    static void drawTitle(const char* title);
    static void drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint8_t scroll_offset);
    static void drawRSSIBar(uint8_t percentage, int8_t rssi_dbm);
    static void drawMACAddress(const uint8_t* mac);
    
private:
    static void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    static void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
    static void drawText(uint16_t x, uint16_t y, const char* text, uint16_t color, uint8_t size);
    
    // Color definitions (Cool theme)
    static constexpr uint16_t COLOR_BLACK = 0x0000;
    static constexpr uint16_t COLOR_WHITE = 0xFFFF;
    static constexpr uint16_t COLOR_DARK_BLUE = 0x0010;
    static constexpr uint16_t COLOR_CYAN = 0x07FF;
    static constexpr uint16_t COLOR_LIGHT_BLUE = 0x841F;
    static constexpr uint16_t COLOR_GREEN = 0x07E0;
};

#endif // DISPLAY_MANAGER_H