#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "config.h"

class DisplayManager {
public:
    static void init();
    static void showSplash(); // Hiện logo lúc khởi động
    static void drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint16_t selected_idx);
    static void drawTracker(WiFiDevice_t* device); // Chế độ săn tìm
    static void clear();

private:
    static TFT_eSPI tft;
    // Bảng màu hiện đại
    static constexpr uint16_t COL_BACK = 0x0000; // Đen
    static constexpr uint16_t COL_MAIN = 0x07FF; // Cyan
    static constexpr uint16_t COL_ACCENT = 0xF81F; // Hồng tím
};

#endif
