# 📱 Phone Detector - WiFi Sniffer

**ESP32-S3 N16R8 Firmware cho theo dõi thiết bị di động trong trường học**

> Một hệ thống nhúng chuyên dụng quét Wi-Fi Promiscuous, theo dõi RSSI thời gian thực, và phân loại thiết bị di động dựa trên MAC OUI.

---

## 🎯 Tính Năng

### 1. **Mode DISCOVERY** 
- ✅ Quét liên tục 13 kênh Wi-Fi (1-13 2.4GHz)
- ✅ Hiển thị danh sách thiết bị phát hiện với:
  - Tên nhà sản xuất (Apple, Samsung, Xiaomi, Oppo, Vivo)
  - 3 byte cuối MAC address
  - RSSI hiện tại (dBm)
- ✅ Cuộn danh sách bằng 2 nút Up/Down
- ✅ Sắp xếp theo RSSI mạnh nhất

### 2. **Mode TARGET TRACKING**
- ✅ Theo dõi 1 thiết bị cụ thể sau khi chọn
- ✅ Hiển thị thanh Progress Bar cho RSSI
  - Range: -30dBm (gần) → -90dBm (xa)
  - Màu sắc: Đỏ (xa) → Vàng → Xanh lá (gần)
- ✅ Buzzer phát âm tỉ lệ với RSSI:
  - **Gần**: Tần số cao (2000Hz), duty cycle thấp (30%)
  - **Xa**: Tần số thấp (500Hz), duty cycle cao (80%)

### 3. **Signal Processing**
- ✅ **Moving Average Filter** (32 samples) làm mượt RSSI
- ✅ **Tự động đặt tên thiết bị**: iPhone 1, iPhone 2, Samsung 1, ...
- ✅ **OUI Database**: Tra cứu 80+ mã OUI của 5 hãng phổ biến

### 4. **Điều khiển Phần cứng**
- ✅ **3 nút điều khiển**: Pin 40, 41, 42 (GPIO)
- ✅ **Buzzer PWM**: Pin GPIO 10, LEDC Channel 0
- ✅ **TFT Display**: 1.54 inch (240x135), I2C/SPI
- ✅ **PSRAM**: 8MB cho lưu trữ danh sách thiết bị

---

## 📋 Thông Số Kỹ Thuật

| Thông Số | Giá Trị |
|---------|--------|
| **MCU** | ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM) |
| **Wi-Fi** | 802.11b/g/n 2.4GHz, Promiscuous Mode |
| **Memory** | 512KB SRAM + 8MB PSRAM (ps_malloc) |
| **Display** | TFT 1.54" (240x135 pixels) |
| **Update Rate** | 500ms (display), 100ms (channel hop) |
| **Max Devices** | 200 (PSRAM) |
| **RSSI Range** | -30dBm đến -90dBm |

---

## 🔌 Sơ Đồ Chân

### **Display (TFT 1.54")**
| Chân | GPIO | Mô Tả |
|-----|------|-------|
| SCL | 20 | I2C Clock |
| SDA | 21 | I2C Data |
| RST | 47 | Reset |
| DC | 18 | Data/Command |
| CS | 45 | Chip Select |

### **Buttons**
| Nút | GPIO | Chức Năng |
|-----|------|----------|
| UP | 40 | Cuộn lên |
| DOWN | 42 | Cuộn xuống |
| SELECT | 41 | Chọn / Toggle mode |

### **Buzzer**
| Thành Phần | GPIO | Mô Tả |
|-----------|------|-------|
| Buzzer | 10 | LEDC PWM Output |

---

## 📂 Cấu Trúc Dự Án
