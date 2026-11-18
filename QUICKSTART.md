# Quick Start Guide - ESP32 + 74HC595 + MQTT

## 🚀 Bước 1: Chuẩn bị môi trường (5 phút)

```bash
# 1. Cài PlatformIO (nếu chưa)
pip3 install platformio

# 2. Vào thư mục dự án
cd 74hc595

# 3. Cài VS Code extension (nếu dùng)
code --install-extension platformio.platformio-ide
```

## 📝 Bước 2: Cấu hình (2 phút)

Chỉnh `include/config.h`:
- `WIFI_SSID = "Wokwi-GUEST"` (mạng Wi-Fi của bạn)
- `MQTT_HOST = "broker.emqx.io"` (không cần đổi nếu dùng public)
- `DEVICE_ID = "node-A"` (tên device)
- `PIN_SER = 14`, `PIN_SRCLK = 12`, `PIN_RCLK = 13` (GPIO)

## 🔨 Bước 3: Build (2 phút lần đầu, 30s sau)

```bash
# Lần đầu (tải SDK)
export PATH="$PATH:/Users/vohoanglong/Library/Python/3.9/bin"
pio run

# Lần sau
pio run

# Output: .pio/build/esp32doit-devkit-v1/firmware.elf
```

## 📤 Bước 4: Upload lên ESP32 (2 phút)

```bash
# Nối ESP32 qua USB
pio run --target upload

# Nếu không tìm cổng (Windows):
#   pio device list
#   pio run -e esp32doit-devkit-v1 --upload-port COM5 --target upload
```

## 👀 Bước 5: Xem log (kiểm tra kết nối)

```bash
# Terminal 1: Monitor
pio device monitor --baud 115200

# Mong đợi:
# === ESP32 + 74HC595 + MQTT Demo ===
# WiFi connected! IP: 192.168.x.x
# MQTT connected!
# Subscribed to: warehouse/node-A/light
```

## 🧪 Bước 6: Test MQTT (3 phút)

### Cách A: CLI (nhanh nhất)
```bash
# Terminal 2: Publish lệnh
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"on","ledIndex":0}'

# Nhìn Terminal 1 (monitor): Thấy [LED] ON #0 → Thành công!
```

### Cách B: Python (nếu cài paho-mqtt)
```bash
pip install paho-mqtt
python test_mqtt.py
```

### Cách C: Script interactif
```bash
bash test_mqtt.sh  # (macOS/Linux)
```

## 🎮 Demo Wokwi (optional, nhưng rất hay!)

```bash
# Mở VS Code
code .

# Nhấn Ctrl+Shift+P → "Wokwi: Start Simulator"
# Hoặc mở File → Open in Wokwi

# Wokwi sẽ:
# 1. Load diagram.json (2 chip + 16 LED)
# 2. Upload firmware vào ESP32 sim
# 3. Giả lập Wi-Fi + MQTT

# Rồi test MQTT như bước 6 → nhìn LED nhấp nháy trên màn hình!
```

## 📡 Payload JSON nhanh

| Lệnh | Payload | Kết quả |
|------|---------|--------|
| Bật LED 0 | `{"action":"on","ledIndex":0}` | LED 0 sáng |
| Tắt LED 0 | `{"action":"off","ledIndex":0}` | LED 0 tắt |
| Tắt tất cả | `{"action":"clear"}` | Tắt 16 LED |
| Bitmask | `{"action":"set","mask":255}` | Bật LED 0-7 |
| Nhiều kênh | `{"action":"setMany","channels":[1,5,10],"state":true}` | Bật 3 kênh |

## 🆘 Troubleshoot

| Vấn đề | Giải pháp |
|--------|----------|
| pio: command not found | `export PATH="$PATH:/Users/vohoanglong/Library/Python/3.9/bin"` |
| ESP32 not found | Cắm lại USB, check `pio device list` |
| WiFi failed | Kiểm tra SSID/password, Wokwi phải có mạng |
| MQTT fail rc=-4 | Broker không kết nối → `ping broker.emqx.io` |
| LED không sáng (Wokwi) | Kiểm tra wokwi.json connections |
| JSON parse error | Payload có lỗi syntax → test trên jsonlint.com |

## 📚 File quan trọng

- `include/config.h` ← **Cấu hình: Wi-Fi, MQTT, GPIO**
- `src/main.cpp` ← **Code chính (all logic)**
- `platformio.ini` ← **Build settings**
- `wokwi.json` ← **Sơ đồ Wokwi**
- `README.md` ← **Hướng dẫn chi tiết**

## ✅ Checklist trước demo

- [ ] PlatformIO cài xong
- [ ] config.h sửa xong (Wi-Fi, MQTT)
- [ ] `pio run` build thành công
- [ ] ESP32 upload xong
- [ ] Serial Monitor thấy "Setup complete!"
- [ ] `mosquitto_pub` publish thành công
- [ ] LED nhấp nháy trên Wokwi hoặc kit thật

---

**Done? Start Wokwi Demo! 🚀**

Mở file này (QUICKSTART.md) ở VS Code → Lên menu Wokwi → Click nút Start Simulator

Hoặc chạy terminal:
```bash
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light -m '{"action":"clear"}' && \
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light -m '{"action":"set","mask":255}'
```
