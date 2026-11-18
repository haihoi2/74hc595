# 📖 Index — Danh sách tất cả file và cách sử dụng

## 🗂️ Cấu trúc thư mục

```
74hc595/
│
├── 📂 include/
│   └── config.h                    ← [CHỈNH SỬA] Cấu hình Wi-Fi, MQTT, GPIO
│
├── 📂 src/
│   └── main.cpp                    ← [ĐỌC] Code chính (tất cả logic)
│
├── 📂 .pio/
│   └── build/esp32doit-devkit-v1/
│       ├── firmware.elf            ← Executable (11MB)
│       ├── firmware.bin            ← Binary cho flash (884KB)
│       └── firmware.map            ← Memory map
│
├── 📂 lib/
│   └── (libraries sẽ được tải tự động)
│
├── 📋 platformio.ini               ← [CHỈNH SỬA] Build settings + dependencies
├── 📋 wokwi.json                   ← [CHỈNH SỬA] Sơ đồ Wokwi simulator
│
├── 📚 README.md                    ← [ĐỌC TRƯỚC] Hướng dẫn toàn diện
├── 📚 CODE_STRUCTURE.md            ← [ĐỌC] Giải thích chi tiết từng hàm
├── 📚 QUICKSTART.md                ← [ĐỌC] Bắt đầu nhanh (5 phút)
├── 📚 BUILD_SUMMARY.md             ← [ĐỌC] Tóm tắt build + stats
├── 📚 INDEX.md                     ← File này
│
├── 🧪 test_mqtt.sh                 ← [CHẠY] Bash script test MQTT
├── 🧪 test_mqtt.py                 ← [CHẠY] Python script test MQTT
├── 🧪 verify_code.sh               ← [CHẠY] Verification script
│
└── 📄 esp32-74hc595-mqtt.html      ← Slide gốc (tham khảo)
```

---

## 🎯 Cách sử dụng từng file

### 📄 Code Files

#### `include/config.h` ⭐ BẮT BUỘC CHỈNH SỬA
**Mục đích**: Khai báo tất cả hằng số cấu hình

**Cần chỉnh sửa nếu**:
- Dùng Wi-Fi khác (không phải Wokwi-GUEST)
- Dùng broker MQTT khác (không phải broker.emqx.io)
- GPIO của ESP32 khác
- Số chip/LED khác

**Ví dụ**:
```cpp
#define WIFI_SSID       "MyNetwork"      // Thay tên mạng
#define WIFI_PASSWORD   "password123"    // Mật khẩu
#define MQTT_HOST       "192.168.1.100"  // IP Mosquitto cục bộ
#define DEVICE_ID       "node-B"         // Tên device khác
```

---

#### `src/main.cpp` 📖 ĐỌC & HỌC
**Mục đích**: Code chính

**Cấu trúc**:
```cpp
// 7 phần chính:
// 1️⃣ writeShift(mask) → Ghi vào 74HC595
// 2️⃣ setLed(index, on) → Bật/tắt 1 LED
// 3️⃣ clearAll() → Tắt hết
// 4️⃣ connectWifi() → Kết nối mạng
// 5️⃣ connectMqtt() → Kết nối broker
// 6️⃣ handleMqtt() → Xử lý lệnh MQTT
// 7️⃣ setup() / loop() → Arduino main
```

**Học:**
- Cách ghi shift register (digitalWrite + shiftOut)
- Parsing JSON (ArduinoJson)
- Callback pattern (handleMqtt)
- State management (ledMask)

---

#### `platformio.ini` ⚙️ BUILD CONFIG
**Mục đích**: Cấu hình build & dependencies

**Chỉnh sửa nếu**:
- Đổi board (ESP32-S3, NodeMCU, v.v)
- Thêm/bỏ library
- Thay đổi baud rate
- Tối ưu hoá

**Ví dụ**:
```ini
[env:esp32doit-devkit-v1]
board = esp32doit-devkit-v1  ← Thay board ở đây

[env:esp32-s3-devkit-c-1]    ← Thêm environment mới
platform = espressif32
board = esp32-s3-devkit-c-1
```

---

#### `wokwi.json` 🎮 SIMULATOR DIAGRAM
**Mục đích**: Sơ đồ cho Wokwi simulator

**Chỉnh sửa nếu**:
- Thêm/bỏ LED
- Đổi GPIO mapping
- Thêm component (button, sensor, v.v)
- Thay đổi vị trí trên diagram

**Ví dụ cấu trúc**:
```json
{
  "parts": [
    {"type": "wokwi-esp32-devkit-v1", "id": "esp32", ...},
    {"type": "wokwi-74hc595", "id": "sr1", ...},
    {"type": "wokwi-led", "id": "led0", ...}
  ],
  "connections": [
    ["esp32:14", "sr1:14", "blue", ...],  // GPIO14 → SR1 DS
    ...
  ]
}
```

---

### 📚 Documentation Files

#### `README.md` 📖 START HERE
**Nội dung**:
- Mục lục
- Mở đầu (tại sao dùng 74HC595?)
- Cấu trúc dự án
- Phần cứng (sơ đồ + danh sách linh kiện)
- Cấu hình (chỉnh config.h, platformio.ini)
- Cơ chế hoạt động (luồng shift out, MQTT)
- Build & Upload
- Demo Wokwi
- Troubleshoot
- Các ví dụ MQTT (bash, Python, Node.js)
- Bài tập

**Khi nào đọc**: Lần đầu, để hiểu tổng quan

---

#### `CODE_STRUCTURE.md` 🧠 HIỂU CODE
**Nội dung**:
- Sơ đồ kiến trúc
- Giải thích chi tiết 7 hàm chính
- Luồng hoạt động (startup → loop → handleMqtt)
- Mapping LED ↔ Bit
- Luồng dữ liệu từ đầu tới cuối
- Điểm chính (key takeaways)
- Bài tập hiểu code

**Khi nào đọc**: Khi muốn hiểu chi tiết từng hàm, trước khi modify code

---

#### `QUICKSTART.md` ⚡ 5 PHÚT BẮTĐẦU
**Nội dung**:
- Bước 1: Setup (cài PlatformIO)
- Bước 2: Cấu hình (config.h)
- Bước 3: Build (pio run)
- Bước 4: Upload (pio run --target upload)
- Bước 5: Monitor (xem log)
- Bước 6: Test MQTT (3 cách)
- Troubleshoot

**Khi nào đọc**: Lần đầu, để bắt đầu nhanh

---

#### `BUILD_SUMMARY.md` 📊 BUILD REPORT
**Nội dung**:
- Trạng thái build (SUCCESS ✓)
- Thống kê (RAM/Flash usage)
- Các file được tạo
- Cấu trúc code (7 hàm)
- Dependencies
- Performance metrics
- Test cases
- Known issues
- Mở rộng trong tương lai
- Checklist demo

**Khi nào đọc**: Sau khi build xong, để xem stats

---

#### `INDEX.md` 📖 FILE NÀY
**Nội dung**: Danh sách tất cả file + cách sử dụng

**Khi nào đọc**: Khi không biết tìm file gì

---

### 🧪 Test Files

#### `test_mqtt.sh` (Bash)
**Mục đích**: Test MQTT từ command line

**Cách chạy**:
```bash
bash test_mqtt.sh
# Menu interactif xuất hiện
```

**Lệnh nhanh**:
```bash
# Bật LED 0
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"on","ledIndex":0}'

# Tắt tất cả
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"clear"}'
```

---

#### `test_mqtt.py` (Python)
**Mục đích**: Test MQTT từ Python

**Cách chạy**:
```bash
pip install paho-mqtt  # Lần đầu
python test_mqtt.py
# Menu interactif
```

**Yêu cầu**: Python 3.6+

---

#### `verify_code.sh`
**Mục đích**: Kiểm tra xem code structure đúng

**Cách chạy**:
```bash
bash verify_code.sh
# Output: Check tất cả file + hàm
```

---

### 📄 Reference Files

#### `esp32-74hc595-mqtt.html`
**Mục đích**: Slide gốc (PowerPoint HTML)

**Cách xem**:
```bash
open esp32-74hc595-mqtt.html  # macOS
# hoặc drag vào browser
```

**Nội dung**: 16 slide slide về 74HC595, MQTT, firmware

---

## 🚀 Workflow Tiêu Chuẩn

### Lần đầu chạy

```bash
1. cd 74hc595
2. Đọc README.md (5 phút)
3. Chỉnh sửa config.h (Wi-Fi, MQTT)
4. Build: pio run
5. Upload: pio run --target upload
6. Monitor: pio device monitor
7. Test MQTT: bash test_mqtt.sh
```

### Mở rộng / Debug

```bash
1. Đọc CODE_STRUCTURE.md (10 phút)
2. Chỉnh sửa src/main.cpp
3. Build lại: pio run
4. Monitor: pio device monitor
5. Xem log → tìm bug
```

### Mô phỏng (không cần ESP32 thật)

```bash
1. code .  # Mở VS Code
2. Ctrl+Shift+P → "Wokwi: Start Simulator"
3. Diagram load (ESP32 + 74HC595 + 16 LED)
4. Code chạy trong simulator
5. Test: bash test_mqtt.sh
   → Nhìn LED nhấp nháy trên màn hình!
```

---

## 📌 File nào cần chỉnh sửa?

| Trường hợp | File | Action |
|-----------|------|--------|
| Đổi Wi-Fi | config.h | Chỉnh WIFI_SSID, WIFI_PASSWORD |
| Dùng Mosquitto cục bộ | config.h | Chỉnh MQTT_HOST = IP broker |
| Đổi GPIO | config.h | Chỉnh PIN_SER, PIN_SRCLK, PIN_RCLK |
| Thêm logic LED | src/main.cpp | Thêm action trong handleMqtt() |
| Thay đổi build | platformio.ini | Chỉnh board, lib_deps |
| Cập nhật diagram | wokwi.json | Chỉnh parts / connections |
| Học code | CODE_STRUCTURE.md | Đọc chi tiết |
| Debug | README.md → Troubleshoot | Tìm giải pháp |

---

## 🎓 Learning Path

### Path 1: User (chạy code)
```
1. README.md (20 phút)
2. QUICKSTART.md (5 phút)
3. pio run + test MQTT (10 phút)
4. Done! ✓
```

### Path 2: Developer (sửa code)
```
1. README.md (20 phút)
2. CODE_STRUCTURE.md (30 phút)
3. pio run (5 phút)
4. Modify src/main.cpp (30+ phút)
5. Debug + repeat
```

### Path 3: Engineer (tích hợp)
```
1. CODE_STRUCTURE.md (30 phút)
2. platformio.ini - đổi board (10 phút)
3. wokwi.json - sơ đồ mới (20 phút)
4. Test trên kit thật (60+ phút)
5. Deploy / Optimize
```

---

## 🔗 External References

| Liên kết | Nội dung |
|---------|---------|
| README.md → Phần cứng | Sơ đồ nối chân |
| CODE_STRUCTURE.md → Luồng hoạt động | Diagram tổng quan |
| platformio.ini → Datasheet | Link Arduino setup |
| esp32-74hc595-mqtt.html | Slide gốc (16 slide) |

---

## ✅ Checklist trước demo

- [ ] Đã đọc README.md?
- [ ] config.h sửa đúng Wi-Fi + MQTT?
- [ ] `pio run` build thành công?
- [ ] Upload xong, serial monitor chạy?
- [ ] MQTT subscribe log thấy được?
- [ ] `test_mqtt.sh` publish thành công?
- [ ] Wokwi mô phỏng LEDnhấp nháy?
- [ ] CODE_STRUCTURE.md hiểu hết 7 hàm?
- [ ] Có câu hỏi gì chưa?

---

## 📞 Liên hệ / Support

**Vấn đề**: Đọc tài liệu tương ứng

| Vấn đề | Tài liệu |
|--------|----------|
| "Làm sao để chạy?" | QUICKSTART.md |
| "Code này làm cái gì?" | CODE_STRUCTURE.md |
| "Lỗi gì vậy?" | README.md → Troubleshoot |
| "Làm sao mở rộng?" | BUILD_SUMMARY.md → Mở rộng |
| "Sơ đồ nối chân?" | README.md → Phần cứng |
| "Payload JSON format?" | README.md → Các ví dụ MQTT |

---

**Version**: 1.0  
**Last update**: 2026-11-18  
**Status**: ✅ Ready to use
