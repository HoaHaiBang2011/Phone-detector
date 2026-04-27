#include "display_manager.h"

TFT_eSPI DisplayManager::tft = TFT_eSPI();

void DisplayManager::init() {
    tft.init();
    tft.setRotation(0); // Chỉnh lại theo hướng lắp board của bồ
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void DisplayManager::showSplash() {
    tft.fillScreen(TFT_BLACK);
    tft.drawRoundRect(10, 10, 220, 220, 10, COL_MAIN);
    tft.setTextDatum(MC_DATUM); // Căn giữa
    tft.drawString("WIFI SNIFFER v1.0", 120, 100, 4);
    tft.drawString("By Gemini & Bồ", 120, 140, 2);
    delay(2000);
}

void DisplayManager::drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint16_t selected_idx) {
    // Vẽ tiêu đề
    tft.fillRect(0, 0, 240, 30, COL_MAIN);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("DANH SACH THIET BI", 120, 15, 2);
    
    if (count == 0) {
        tft.setTextColor(TFT_WHITE);
        tft.drawString("Dang quet sóng...", 120, 120, 2);
        return;
    }

    // Chỉ vẽ 8 thiết bị mỗi lần để tránh tràn màn hình
    int start = (selected_idx / 8) * 8;
    for (int i = 0; i < 8; i++) {
        int idx = start + i;
        int y = 40 + (i * 24);
        
        if (idx < count) {
            // Highlight dòng đang chọn
            if (idx == selected_idx) tft.fillRect(0, y-2, 240, 22, 0x3186); // Xanh xám
            else tft.fillRect(0, y-2, 240, 22, TFT_BLACK);

            tft.setTextColor(TFT_WHITE);
            // Hiện: MAC cuối - Tên NSX - RSSI
            char buf[32];
            snprintf(buf, sizeof(buf), "[%02X:%02X] %-10s %ddB", 
                     devices[idx].mac[4], devices[idx].mac[5], 
                     devices[idx].oui_name, devices[idx].rssi_sum);
            tft.drawString(buf, 10, y, 2);
        }
    }
}

void DisplayManager::drawTracker(WiFiDevice_t* device) {
    if (!device) return;

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(COL_MAIN);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("dang theo dau:", 120, 20, 2);
    
    // Hiện MAC to rõ
    char macStr[20];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             device->mac[0], device->mac[1], device->mac[2],
             device->mac[3], device->mac[4], device->mac[5]);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(macStr, 120, 50, 2);

    // Vẽ thanh RSSI Bar 240x240
    int barWidth = map(device->rssi_sum, RSSI_FAR, RSSI_NEAR, 0, 200);
    barWidth = constrain(barWidth, 0, 200);
    
    tft.drawRect(20, 100, 204, 40, TFT_WHITE); // Khung ngoài
    
    uint16_t color = TFT_GREEN;
    if (device->rssi_sum < -75) color = TFT_RED;
    else if (device->rssi_sum < -60) color = TFT_YELLOW;

    tft.fillRect(22, 102, barWidth, 36, color);
    tft.fillRect(22 + barWidth, 102, 200 - barWidth, 36, TFT_BLACK); // Xóa phần dư

    // Hiện số dBm to ở giữa
    tft.setTextColor(TFT_WHITE);
    tft.drawNumber(device->rssi_sum, 120, 160, 6);
    tft.drawString("dBm", 180, 185, 2);
}
