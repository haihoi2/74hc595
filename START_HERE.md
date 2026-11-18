# 🚀 START HERE — Hướng dẫn 5 phút bắt đầu

## 👋 Chào mừng!

Bạn vừa nhận được một dự án ESP32 + 74HC595 + MQTT hoàn chỉnh!

**Trạng thái**: ✅ Build SUCCESS, Ready to use

---

## 📚 Chọn con đường của bạn

### 🏃 **Cách nhanh nhất (5 phút)**
Bạn chỉ muốn thấy nó chạy?

1. Mở VS Code: `code .`
2. Nhấn Ctrl+Shift+P → `Wokwi: Start Simulator`
3. Chờ load xong (thấy 16 LED)
4. Terminal khác:
   ```bash
   mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
     -m '{"action":"set","mask":255}'
   ```
5. 🎉 Nhìn Wokwi: 8 LED đầu sáng lên!

**→ Đọc**: `WOKWI_GUIDE.md`

---

### 🎓 **Cách học (30 phút)**
Bạn muốn hiểu từng dòng code?

1. Đọc `README.md` (20 phút) — hiểu tổng quan
2. Đọc `CODE_STRUCTURE.md` (10 phút) — hiểu chi tiết 7 hàm
3. Chỉnh sửa `src/main.cpp` hoặc `include/config.h`
4. Build lại: `pio run`

**→ Đọc**: `README.md` → `CODE_STRUCTURE.md`

---

### 🛠️ **Cách deploy (60 phút)**
Bạn có ESP32 thật + kit hardware?

1. Chỉnh `config.h` (Wi-Fi, MQTT)
2. `pio run` → build
3. `pio run --target upload` → flash lên ESP32
4. `pio device monitor` → xem serial log
5. `test_mqtt.sh` → test MQTT

**→ Đọc**: `QUICKSTART.md` → `README.md`

---

## 📖 Danh sách tài liệu

| File | Mục đích | Thời gian |
|------|---------|----------|
| **START_HERE.md** | File này 👈 | 2 phút |
| **WOKWI_GUIDE.md** | Chạy simulator | 5 phút |
| **QUICKSTART.md** | Build + upload | 10 phút |
| **README.md** | Toàn diện | 30 phút |
| **CODE_STRUCTURE.md** | Chi tiết code | 30 phút |
| **BUILD_SUMMARY.md** | Build report | 10 phút |
| **INDEX.md** | File + cách sử dụng | 5 phút |

---

## 🎯 Nhanh chóng: Các lệnh thường dùng

### Build
```bash
export PATH="$PATH:/Users/vohoanglong/Library/Python/3.9/bin"
pio run
```

### Upload (ESP32 kết nối USB)
```bash
pio run --target upload
```

### Monitor Serial
```bash
pio device monitor --baud 115200
```

### Test MQTT
```bash
# Bật LED 0
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"on","ledIndex":0}'

# Tắt tất cả
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"clear"}'

# Interactive menu
bash test_mqtt.sh
```

### Wokwi Simulator
```bash
code .
# Ctrl+Shift+P → Wokwi: Start Simulator
```

---

## 📂 Cấu trúc thư mục

```
74hc595/
├── 📚 Tài liệu (bạn ở đây)
│   ├── START_HERE.md ← File này
│   ├── WOKWI_GUIDE.md
│   ├── README.md
│   ├── QUICKSTART.md
│   ├── CODE_STRUCTURE.md
│   ├── BUILD_SUMMARY.md
│   └── INDEX.md
│
├── 💻 Code
│   ├── include/config.h ← Cấu hình
│   ├── src/main.cpp ← Logic chính
│   └── platformio.ini ← Build config
│
├── 🎮 Simulator
│   ├── diagram.json ← Wokwi diagram
│   └── wokwi.json ← (backup)
│
├── 🧪 Test tools
│   ├── test_mqtt.sh
│   ├── test_mqtt.py
│   └── verify_code.sh
│
└── 📦 Build output
    └── .pio/build/.../firmware.bin ← Upload cái này lên ESP32
```

---

## ⚡ Có vấn đề?

| Vấn đề | Giải pháp |
|-------|----------|
| "Simulator không load" | Xem `WOKWI_GUIDE.md` → Troubleshoot |
| "Code sao vậy?" | Xem `CODE_STRUCTURE.md` |
| "Build lỗi?" | Xem `QUICKSTART.md` → Troubleshoot |
| "MQTT không kết nối" | Xem `README.md` → Troubleshoot |
| "File nào?" | Xem `INDEX.md` |

---

## 🏆 3 Mục tiêu chính

✅ **Hiểu được**:
- Shift register 74HC595 (daisy-chain)
- MQTT pub/sub + JSON
- Arduino firmware (setup/loop/callback)

✅ **Chạy được**:
- Build & flash trên ESP32
- Mô phỏng trên Wokwi
- Điều khiển LED qua MQTT

✅ **Mở rộng được**:
- Thêm sensor
- Thêm action mới
- Integrate với home automation

---

## 🎓 Learning Path

### Mức 1: User (20 phút)
```
1. README.md (Intro)
2. QUICKSTART.md (Build & Upload)
3. test_mqtt.sh (Test)
4. ✓ Thành công!
```

### Mức 2: Developer (60 phút)
```
1. CODE_STRUCTURE.md (Học từng hàm)
2. Modify src/main.cpp
3. pio run + test
4. ✓ Thành công!
```

### Mức 3: Engineer (2+ giờ)
```
1. Tích hợp sensor/relay
2. Optimize firmware
3. Deploy on production
4. ✓ Thành công!
```

---

## 🚀 Bắt đầu ngay bây giờ!

Chọn **một** trong ba bước sau:

### ✨ **Bước 1: Chạy Simulator (5 phút)**
```bash
code .
# Ctrl+Shift+P → Wokwi: Start Simulator

# Terminal khác:
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"set","mask":255}'
```
👉 **Xem**: `WOKWI_GUIDE.md`

### 🔧 **Bước 2: Build & Upload (20 phút)**
```bash
export PATH="$PATH:/Users/vohoanglong/Library/Python/3.9/bin"
pio run
# Nối ESP32 USB
pio run --target upload
```
👉 **Xem**: `QUICKSTART.md`

### 🧠 **Bước 3: Hiểu Code (60 phút)**
```bash
# Đọc chi tiết
cat README.md
cat CODE_STRUCTURE.md
# Modify
nano src/main.cpp
# Build lại
pio run
```
👉 **Xem**: `CODE_STRUCTURE.md`

---

## 📞 Cần giúp?

Tất cả câu hỏi của bạn đều có câu trả lời trong tài liệu:

| Câu hỏi | Tài liệu |
|--------|----------|
| "Làm sao chạy?" | `QUICKSTART.md` |
| "Code gì?" | `CODE_STRUCTURE.md` |
| "Sơ đồ nối?" | `README.md` |
| "MQTT?" | `README.md` - Các ví dụ |
| "Wokwi?" | `WOKWI_GUIDE.md` |
| "File nào?" | `INDEX.md` |
| "Build khó?" | `BUILD_SUMMARY.md` |

---

## ✅ Checklist Bắt đầu

- [ ] Đã cài PlatformIO? (hoặc VS Code + extension)
- [ ] Đã đọc START_HERE.md? (file này)
- [ ] Chọn được con đường (nhanh/học/deploy)?
- [ ] Sẵn sàng bắt đầu?

---

**Chúc bạn thành công! 🚀**

Tiếp theo → Chọn một file từ danh sách tài liệu ở trên

Hoặc chạy Wokwi ngay: `code .` → Ctrl+Shift+P → Wokwi: Start Simulator

---

**Version**: 1.0  
**Status**: ✅ Ready to use  
**Build date**: 2026-11-18
