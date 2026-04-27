#include "display_manager.h"

TFT_eSPI DisplayManager::tft = TFT_eSPI();
TFT_Sprite DisplayManager::img = TFT_Sprite(&tft);

void DisplayManager::init() {
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    
    // Khởi tạo Sprite 240x240. 
    // ESP32-S3 có PSRAM nên chúng ta tận dụng để không tốn RAM nội bộ.
    if (img.createSprite(240, 240) == nullptr) {
        // Nếu không đủ RAM thường, nó sẽ tự tìm trong PSRAM nếu sdkconfig đã bật
        Serial.println("Sprite created!"); 
    }
}

void DisplayManager::showSplash() {
    img.fillSprite(TFT_BLACK); // Xóa sạch khung hình
    img.drawRoundRect(10, 10, 220, 220, 10, COL_MAIN);
    img.setTextDatum(MC_DATUM);
    img.setTextColor(TFT_WHITE);
    img.drawString("WIFI SNIFFER v1.1", 120, 100, 4);
    img.drawString("Double Buffering Enabled", 120, 140, 2);
    img.pushSprite(0, 0); // Đẩy khung hình ra màn hình
    delay(2000);
}

void DisplayManager::drawDeviceList(WiFiDevice_t* devices, uint16_t count, uint16_t selected_idx) {
    img.fillSprite(TFT_BLACK); // XÓA SẠCH TRƯỚC KHI VẼ
    
    // Vẽ tiêu đề cố định
    img.fillRect(0, 0, 240, 30, COL_MAIN);
    img.setTextColor(TFT_BLACK);
    img.setTextDatum(MC_DATUM);
    img.drawString("SCANNING... (" + String(count) + ")", 120, 15, 2);
    
    if (count == 0) {
        img.setTextColor(TFT_WHITE);
        img.drawString("No devices found", 120, 120, 2);
    } else {
        img.setTextDatum(TL_DATUM); // Căn lề trái
        for (int i = 0; i < 8; i++) { // Hiển thị tối đa 8 dòng
            int idx = i; 
            if (idx >= count) break;
            
            int y = 40 + (i * 25);
            
            // Highlight dòng đang chọn
            if (idx == selected_idx) {
                img.fillRect(5, y - 2, 230, 22, COL_MAIN);
                img.setTextColor(TFT_BLACK);
            } else {
                img.setTextColor(TFT_WHITE);
            }
            
            char buf[40];
            snprintf(buf, sizeof(buf), "[%02X:%02X] %-12s %ddB", 
                     devices[idx].mac[4], devices[idx].mac[5], 
                     devices[idx].oui_name, devices[idx].rssi);
            img.drawString(buf, 10, y, 2);
        }
    }
    img.pushSprite(0, 0); // Đẩy toàn bộ nội dung lên màn hình một lần duy nhất
}

void DisplayManager::drawTracker(WiFiDevice_t* device) {
    if (!device) return;

    img.fillSprite(TFT_BLACK); // XÓA SẠCH
    
    img.setTextColor(COL_MAIN);
    img.setTextDatum(TC_DATUM);
    img.drawString("TARGET TRACKING", 120, 20, 2);
    
    // Thông tin thiết bị
    char macStr[20];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             device->mac[0], device->mac[1], device->mac[2],
             device->mac[3], device->mac[4], device->mac[5]);
    
    img.setTextColor(TFT_WHITE);
    img.drawString(device->oui_name, 120, 45, 4);
    img.drawString(macStr, 120, 75, 2);

    // Vẽ thanh Bar năng động
    int barWidth = map(device->rssi, -100, -30, 0, 200);
    barWidth = constrain(barWidth, 0, 200);
    
    // Khung thanh Bar
    img.drawRect(20, 120, 204, 30, TFT_WHITE);
    // Màu sắc thanh Bar đổi theo độ mạnh yếu
    uint16_t barColor = (barWidth > 150) ? TFT_GREEN : (barWidth > 80 ? TFT_YELLOW : TFT_RED);
    img.fillRect(22, 122, barWidth, 26, barColor);
    
    // Chỉ số RSSI
    img.setTextDatum(MC_DATUM);
    img.drawString(String(device->rssi) + " dBm", 120, 170, 4);
    
    img.pushSprite(0, 0); // Render!
}
