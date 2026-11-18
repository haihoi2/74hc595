# 🎮 Hướng dẫn Wokwi Simulator

## ✅ Điều kiện tiên quyết

- ✓ VS Code đã cài
- ✓ Wokwi Extension đã cài: `wokwi.wokwi-simulator`
- ✓ Code build thành công: `pio run` ✓
- ✓ File `diagram.json` ✓

## 🚀 Cách khởi động Simulator

### Cách 1: Từ VS Code Extension (Recommended)

```
1. Mở VS Code
   code .

2. Mở Command Palette
   Ctrl+Shift+P (Windows/Linux)
   Cmd+Shift+P (Mac)

3. Gõ: "Wokwi: Start Simulator"
   Enter

4. Simulator sẽ load:
   - ESP32 DevKit (giả lập)
   - 2×74HC595 Shift Register
   - 16 LED (đa sắc)
```

### Cách 2: Từ Wokwi Dashboard

```
1. Vào https://wokwi.com
2. Chọn "New Project"
3. Chọn "ESP32"
4. Upload file diagram.json
5. Copy code từ src/main.cpp vào editor
6. Click "Start Simulation"
```

### Cách 3: Dùng command line (nếu cài Wokwi CLI)

```bash
wokwi-cli diagram.json --elf .pio/build/esp32doit-devkit-v1/firmware.elf
```

---

## 🎯 Khi Simulator chạy

### Serial Monitor
```
Bạn sẽ thấy:
  === ESP32 + 74HC595 + MQTT Demo ===
  Connecting to WiFi...
  WiFi connected! IP: 192.168.x.x
  
  MQTT Connecting...
  MQTT connected!
  Subscribed to: warehouse/node-A/light
  
  Setup complete!
```

### Diagram sẽ hiển thị
```
┌─── ESP32 ───┐
│    (3V3)    │
│  (GPIO14→DS)│────┐
│  (GPIO12→CK)│    │
│  (GPIO13→LT)│    │
│    (GND)    │    │
└─────────────┘    │
                   ▼
            ┌───74HC595──┐
   (Q7S)───→│ Chip 1    │
            │ Q0…Q7     │
            └─────▲─────┘
                  │
              (Q7S→DS)
                  │
            ┌─────▼─────┐
            │ Chip 2    │
            │ Q0…Q7     │
            └───────────┘
                  │
   ┌──────┬──────┼──────┬──────┐
   ▼      ▼      ▼      ▼      ▼
  LED0   LED1   LED2  ...    LED15
```

---

## 🧪 Testing di Simulator

### Test 1: Bật LED 0

Terminal 1 (Monitor):
```bash
pio device monitor --baud 115200
# Nếu chạy trong simulator, không thấy log serial
```

Terminal 2 (Test):
```bash
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"on","ledIndex":0}'

# Nhìn Wokwi: LED0 sáng lên (đỏ)
```

### Test 2: Bật tất cả LED

```bash
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"set","mask":65535}'

# Nhìn Wokwi: Tất cả 16 LED sáng (đa sắc)
```

### Test 3: Tắt tất cả

```bash
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"clear"}'

# Nhìn Wokwi: Tất cả LED tắt
```

### Test 4: Bật một số LED

```bash
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"setMany","channels":[0,2,4,6,8,10,12,14],"state":true}'

# Nhìn Wokwi: 8 LED (hàng 1 + hàng 2, xen kẽ) sáng
```

---

## ⚙️ Cấu hình Simulator

### Environment variable (giả lập Wi-Fi)
```json
// trong diagram.json → esp32 attrs
"attrs": {
  "env": "Wokwi-GUEST"  // Mạng W-Fi tự động (Wokwi)
}
```

### MQTT Broker
```cpp
// trong config.h
#define MQTT_HOST "broker.emqx.io"  // Public broker (không cần đặc biệt)
```

Wokwi simulator sẽ tự động kết nối tới broker.emqx.io qua internet.

---

## 🐛 Troubleshoot Simulator

### Lỗi: Simulator không load

**Nguyên nhân 1**: Extension chưa cài
```bash
code --install-extension wokwi.wokwi-simulator
```

**Nguyên nhân 2**: diagram.json không tồn tại
```bash
cp wokwi.json diagram.json
```

**Nguyên nhân 3**: Code chưa build
```bash
pio run  # Build lại
```

### Lỗi: LED không sáng

**Check 1**: Nối chân sai trong diagram.json?
```bash
grep "esp32:14.*sr1:14" diagram.json  # Phải có
```

**Check 2**: Payload JSON sai format?
```bash
echo '{"action":"on","ledIndex":0}' | python -m json.tool
# Phải không lỗi
```

**Check 3**: MQTT connection failed?
```bash
# Terminal khác: test broker
mosquitto_sub -h broker.emqx.io -t "warehouse/node-A/light" &
# Chờ... nếu không lỗi thì broker OK
```

### Lỗi: Serial Monitor không log

**Lý do**: Wokwi simulator có serial tích hợp, không qua USB

**Giải pháp**: 
- Xem serial monitor của Wokwi (nếu có)
- Hoặc đừng quan tâm, test qua MQTT là đủ

---

## 🎓 Các thí nghiệm có thể làm

### Thí nghiệm 1: LED nhấp nháy tuần tự
```bash
for i in {0..15}; do
  mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
    -m "{\"action\":\"on\",\"ledIndex\":$i}"
  sleep 0.5
done

# Nhìn Wokwi: LED0→1→2→...→15 sáng lần lượt
```

### Thí nghiệm 2: LED kỳ dị
```bash
for i in {0..15..2}; do
  channels="[$(echo {0..15..2} | tr ' ' ',')]"
  mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
    -m "{\"action\":\"setMany\",\"channels\":$channels,\"state\":true}"
done

# Nhìn Wokwi: LED 0,2,4,6,8,10,12,14 sáng
```

### Thí nghiệm 3: Tắt dần
```bash
mask=65535  # 0xFFFF = tất cả
while [ $mask -gt 0 ]; do
  mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
    -m "{\"action\":\"set\",\"mask\":$mask}"
  mask=$((mask >> 1))  # Dịch phải 1 bit
  sleep 0.2
done

# Nhìn Wokwi: LED tắt dần (từ phải sang trái)
```

---

## 📊 Performance Metrics (Simulator)

| Metric | Value |
|--------|-------|
| Latency MQTT→LED | 50-200ms (network dependent) |
| LED update | Instant (tuyến tính) |
| CPU usage | ~10% (macOS) |
| Memory | ~200MB (Chromium engine) |
| WiFi simulation | ✓ (Wokwi-GUEST) |
| MQTT connection | ✓ (broker.emqx.io) |

---

## 💾 Save / Export

### Lưu project
```
Nếu dùng Wokwi Dashboard:
  → File menu → Save
  → Project được lưu tại wokwi.com
```

### Export firmware
```bash
# Firmware đã build ở đây:
.pio/build/esp32doit-devkit-v1/firmware.bin

# Copy sang thẻ nhớ ESP32 thật:
esptool.py write_flash 0x0000 firmware.bin
```

---

## ✅ Checklist Demo Wokwi

- [ ] Wokwi Extension cài xong
- [ ] Code build thành công (`pio run`)
- [ ] diagram.json ✓
- [ ] Command Palette → Start Simulator
- [ ] Simulator load (thấy ESP32 + 74HC595 + 16 LED)
- [ ] mosquitto_pub test → LED sáng
- [ ] Clear lệnh → LED tắt
- [ ] Setmany test → LED đúng
- [ ] Lên hình / video demo

---

**Chúc bạn demo thành công! 🚀**

Nếu có vấn đề → Xem README.md → Troubleshoot
