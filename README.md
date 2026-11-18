# ESP32 + 74HC595 + MQTT — Điều khiển 16 kênh LED

## 📋 Mục lục
- [Mở đầu](#mở-đầu)
- [Cấu trúc dự án](#cấu-trúc-dự-án)
- [Phần cứng](#phần-cứng)
- [Cấu hình](#cấu-hình)
- [Cơ chế hoạt động](#cơ-chế-hoạt-động)
- [Build & Upload](#build--upload)
- [Demo với Wokwi](#demo-với-wokwi)
- [Kiểm tra & Debug](#kiểm-tra--debug)
- [Các ví dụ MQTT](#các-ví-dụ-mqtt)

---

## 🎯 Mở đầu

Dự án này minh họa cách **điều khiển 16 kênh LED/relay từ ESP32** bằng:
- **74HC595**: shift register (mở rộng GPIO từ 3 chân thành 16 chân)
- **MQTT**: giao tiếp từ xa qua broker (emqx.io hoặc Mosquitto)
- **JSON**: payload định dạng lệnh

**Lợi ích:**
- Tiết kiệm GPIO ESP32 (chỉ dùng 3 chân: DS, SHCP, STCP)
- Dễ mở rộng (daisy-chain thêm chip)
- Điều khiển từ bất kỳ thiết bị nào có Wi-Fi + MQTT client

---

## 📁 Cấu trúc dự án

```
74hc595/
├── include/
│   └── config.h              ← Cấu hình Wi-Fi, MQTT, GPIO
├── src/
│   └── main.cpp              ← Code chính (tất cả logic)
├── platformio.ini            ← Cấu hình build & dependencies
├── wokwi.json                ← Sơ đồ Wokwi (2×74HC595 + 16 LED)
├── README.md                 ← File này
└── esp32-74hc595-mqtt.html   ← Slide hướng dẫn
```

### File chính

#### **include/config.h**
Khai báo tất cả hằng số:
- Wi-Fi: `WIFI_SSID`, `WIFI_PASSWORD`
- MQTT: `MQTT_HOST`, `MQTT_PORT`, `DEVICE_ID`
- GPIO: `PIN_SER` (14), `PIN_SRCLK` (12), `PIN_RCLK` (13)

#### **src/main.cpp**
Code Arduino chia làm 7 phần:
1. **writeShift(mask)** — ghi 16-bit vào 74HC595
2. **setLed(index, on)** — bật/tắt 1 LED
3. **clearAll()** — tắt tất cả
4. **connectWifi()** — kết nối mạng
5. **handleMqtt()** — xử lý tin MQTT nhận được
6. **connectMqtt()** — đăng ký broker
7. **setup() / loop()** — Arduino main

---

## 🔌 Phần cứng

### Danh sách linh kiện

| Thành phần | Số lượng | Ghi chú |
|-----------|---------|--------|
| ESP32 DevKit | 1 | Hoặc NodeMCU nếu hỗ trợ |
| 74HC595 Shift Register | 2 | Daisy-chain: Q7S chip 1 → DS chip 2 |
| LED đơn sắc | 16 | Màu tuỳ chọn, Vf ≈ 2–2.2V |
| Điện trở LED | 16 | **220–330 Ω** (quan trọng!) |
| Tụ hóa | 2 | 100 nF VCC↔GND gần chip |
| Dây nối | — | Dây cáp đủ dài |

### Sơ đồ kết nối

```
                     ESP32 DevKit
            ┌──────────────────┐
            │ GPIO14 ───┐      │
            │ GPIO12 ───┼─────→ SHCP (Clock)
            │ GPIO13 ───┼─────→ STCP (Latch)
            │ 3V3   ────┼─────→ VCC + SRCLR
            │ GND   ────┼─────→ GND + OE
            └──────────────────┘
                       │
              ┌────────▼────────┐
              │  74HC595 (SR1)  │
    DS ──────▶ DS              │
    SHCP ────▶ SHCP            │ ──┐ Q7S
    STCP ────▶ STCP            │   │
    VCC ─────▶ VCC             │   │
    GND ─────▶ GND + OE        │   │
              └────────┬────────┘   │
                       │             │
                Q0–Q7 ──▶ LED0–7     │
                       │             │
              ┌────────▼────────┐    │
              │  74HC595 (SR2)  │    │
              │                 │◀───┘ (Q7S→DS)
    SHCP ────▶ SHCP            │
    STCP ────▶ STCP            │
    VCC ─────▶ VCC             │
    GND ─────▶ GND + OE        │
              └─────────────────┘
                       │
                Q0–Q7 ──▶ LED8–15
                       │
                      LED (Anode)
                       │
              [220Ω Resistor]
                       │
                      GND
```

### Cảnh báo ⚠️

1. **Luôn có điện trở LED** (220–330Ω) — nếu không → LED kéo quá dòng → 74HC595 nóng/hỏng
2. **Chung GND** — ESP32 và 74HC595 phải chia sẻ mass/GND
3. **Không relay trực tiếp** — relay phải dùng module optocoupler + transistor, không nối vào chân Q
4. **USB ≥1A** — Wi-Fi + 16 LED cùng sáng có thể cần 500mA+

---

## ⚙️ Cấu hình

### Chỉnh sửa config.h

```cpp
// Wi-Fi (Wokwi)
#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASSWORD   ""

// MQTT Broker (thay đổi nếu dùng Mosquitto cục bộ)
#define MQTT_HOST       "broker.emqx.io"    // ← thay IP/domain nếu khác
#define MQTT_PORT       1883                // ← cổng MQTT thường dùng

// Nhân dạng thiết bị (mỗi node một tên khác)
#define DEVICE_ID       "node-A"            // ← "node-A", "node-B", ...

// GPIO pins (tùy sơ đồ của bạn)
#define PIN_SER         14    // Data (DS)
#define PIN_SRCLK       12    // Clock (SHCP)
#define PIN_RCLK        13    // Latch (STCP)
```

### Chỉnh sửa platformio.ini

Nếu dùng board khác:
```ini
[env:esp32doit-devkit-v1]  ← thay thành board của bạn
platform = espressif32
board = esp32doit-devkit-v1  ← "esp32-s3-devkit-c-1", ...
```

---

## 🔧 Cơ chế hoạt động

### Luồng Shift Out (ghi LED)

```
Mask = 0b1010_0101 (16-bit)

Bước 1: Hạ RCLK (Latch)
  RCLK = 0

Bước 2: Đẩy byte cao (chip 2) — MSB first
  Bit 15–8 vào DS, mỗi xung SHCP dịch 1 bit vào shift register

Bước 3: Đẩy byte thấp (chip 1) — MSB first  
  Bit 7–0 vào DS tiếp tục

Bước 4: Lên RCLK (Latch)
  RCLK = 1 → Q0–Q15 cập nhật đồng loạt
  
Kết quả: LED tương ứng sáng/tắt ngay lập tức
```

Hàm:
```cpp
void writeShift(uint16_t mask) {
  digitalWrite(PIN_RCLK, LOW);
  shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST, (mask >> 8) & 0xFF); // Byte cao
  shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST,  mask & 0xFF);       // Byte thấp
  digitalWrite(PIN_RCLK, HIGH);  // Latch
}
```

### Luồng MQTT

```
1. ESP32 khởi động
   → connectWifi()   [lấy IP từ Wokwi-GUEST]
   → connectMqtt()   [kết nối broker.emqx.io:1883]
   → mqtt.subscribe("warehouse/node-A/light")  [nghe topic]

2. Khi App/MQTTX publish JSON lên topic "warehouse/node-A/light"
   → Broker gửi tin tới ESP32
   → Trigger handleMqtt() callback
   → Parse JSON
   → Gọi setLed() / writeShift() / clearAll()
   → LED đổi trạng thái

3. mqtt.loop() trong loop()
   ← BẮT BUỘC ← giữ MQTT alive & nhận tin
```

### Mapping LED ↔ Bit

```
Mask 16-bit:  bit15 bit14 … bit8 │ bit7 bit6 … bit0
              ───────────────────┼──────────────────
              SR2:Q7 … SR2:Q0    │ SR1:Q7 … SR1:Q0
              LED15  … LED8      │ LED7  … LED0

Ví dụ: Bật LED0, LED3, LED7
  mask = (1<<0) | (1<<3) | (1<<7)
       = 0b0000_0000_1010_1001  (0x0A9)

Ghi lên 74HC595:
  writeShift(0x0A9)
  → LED0, LED3, LED7 sáng; lại tắt
```

---

## 🚀 Build & Upload

### Yêu cầu
- **PlatformIO CLI** hoặc **VS Code + PlatformIO extension**
- **ESP32 SDK** (tải tự động)

### Build
```bash
# Vào thư mục dự án
cd 74hc595

# Build firmware
export PATH="$PATH:/Users/vohoanglong/Library/Python/3.9/bin"
pio run

# Kết quả: .pio/build/esp32doit-devkit-v1/firmware.elf
```

### Upload
```bash
# Upload lên ESP32 (cắm USB)
pio run --target upload

# Monitor Serial (debug)
pio device monitor --baud 115200
```

### Xem log
```bash
pio device monitor --baud 115200

# Từ ESP32:
# [MQTT] Topic: warehouse/node-A/light | Length: 30
# [MQTT] Payload: {"action":"on","ledIndex":5}
# [LED] ON #5
```

---

## 🎮 Demo với Wokwi

### Cách chạy

1. **Mở VS Code → Wokwi simulator**
   ```
   Ctrl+Shift+P → "Wokwi: Start Simulator"
   ```

2. **Wokwi tải diagram.json** (đã chuẩn bị)
   - 1 ESP32 DevKit
   - 2 chip 74HC595 (daisy-chain)
   - 16 LED đa sắc

3. **ESP32 bắt đầu chạy code**
   - Kết nối Wi-Fi: `Wokwi-GUEST` (giả lập)
   - Kết nối MQTT: `broker.emqx.io`
   - Subscribe: `warehouse/node-A/light`

4. **Từ máy tính khác: publish MQTT**
   ```bash
   # Bật LED 0
   mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light -m \
     '{"action":"on","ledIndex":0}'
   
   # Nhìn Wokwi: LED0 sáng lên
   ```

### Lệnh test nhanh

```bash
# Cần cài: mosquitto-clients
brew install mosquitto  # macOS
# hoặc apt-get install mosquitto-clients  # Linux

# 1. Bật LED0
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"on","ledIndex":0}'

# 2. Tắt LED0
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"off","ledIndex":0}'

# 3. Tắt tất cả
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"clear"}'

# 4. Bật LED 0,2,4,6 cùng lúc (bitmask)
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"set","mask":85}'

# 5. Bật kênh 1,5,10 (setMany)
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"setMany","channels":[1,5,10],"state":true}'
```

---

## 🔍 Kiểm tra & Debug

### Serial Monitor
```
Mở: pio device monitor --baud 115200

Mong đợi:
  === ESP32 + 74HC595 + MQTT Demo ===
  Connecting to WiFi...........
  WiFi connected! IP: 192.168.x.x
  
  MQTT Connecting... (clientId: shelf-node-A-xxxxx)
  MQTT connected!
  Subscribed to: warehouse/node-A/light
  
  Setup complete!
  
  [MQTT] Topic: warehouse/node-A/light | Length: 30
  [MQTT] Payload: {"action":"on","ledIndex":3}
  [JSON] action: on
  [LED] ON #3
```

### Lỗi thường gặp

| Lỗi | Nguyên nhân | Giải pháp |
|-----|-----------|----------|
| `WiFi failed` | Mạng W-Fi không có | Kiểm tra SSID/password, Wokwi phải có mạng |
| `MQTT connection failed` | Broker không kết nối được | Ping `broker.emqx.io`, hoặc chạy Mosquitto cục bộ |
| `JSON parse failed` | Payload JSON sai | Kiểm tra payload có escaped quotes không |
| LED không sáng | Nối chân sai hoặc LED ngược cực | Kiểm tra wokwi.json + điện trở |
| LED nhấp nháy lạ | Latch pin sai hoặc clock lệch pha | Kiểm tra PIN_RCLK, PIN_SRCLK |

### Giải pháp check

```cpp
// Thêm vào handleMqtt() để debug chi tiết
Serial.printf("[DEBUG] ledMask before: 0x%04X\n", ledMask);
// ... logic ...
Serial.printf("[DEBUG] ledMask after: 0x%04X\n", ledMask);
```

---

## 📡 Các ví dụ MQTT

### Payload JSON — Action duy nhất

```json
// Bật 1 kênh
{ "action": "on", "ledIndex": 7 }

// Tắt 1 kênh
{ "action": "off", "ledIndex": 7 }

// Tắt tất cả
{ "action": "clear" }
```

### Payload JSON — Bitmask (ghi trực tiếp)

```json
// Bật LED 0,1,2,3 (bit 0-3 = 1)
{ "action": "set", "mask": 15 }
// 15 = 0b0000_0000_0000_1111

// Bật LED 0,2,4,6 (pattern xen kẽ)
{ "action": "set", "mask": 85 }
// 85 = 0b0000_0000_0101_0101

// Bật tất cả 16 LED
{ "action": "set", "mask": 65535 }
// 65535 = 0xFFFF
```

### Payload JSON — Danh sách kênh

```json
// Bật kênh 1, 5, 10, 15
{
  "action": "setMany",
  "channels": [1, 5, 10, 15],
  "state": true
}

// Tắt kênh 0, 7, 8
{
  "action": "setMany",
  "channels": [0, 7, 8],
  "state": false
}
```

### Python + paho-mqtt

```python
import paho.mqtt.client as mqtt
import json
import time

def on_connect(client, userdata, flags, rc):
    print(f"Connected with code {rc}")

client = mqtt.Client()
client.on_connect = on_connect
client.connect("broker.emqx.io", 1883, 60)
client.loop_start()

time.sleep(2)

# Bật LED 0
payload = {"action": "on", "ledIndex": 0}
client.publish("warehouse/node-A/light", json.dumps(payload))

time.sleep(1)

# Tắt LED 0
payload = {"action": "off", "ledIndex": 0}
client.publish("warehouse/node-A/light", json.dumps(payload))

time.sleep(1)

# Bật 4 LED cùng lúc
payload = {
    "action": "setMany",
    "channels": [0, 2, 4, 6],
    "state": True
}
client.publish("warehouse/node-A/light", json.dumps(payload))

client.loop_stop()
```

### Node.js + mqtt

```javascript
const mqtt = require('mqtt');

const client = mqtt.connect('mqtt://broker.emqx.io:1883');

client.on('connect', () => {
  console.log('Connected');

  // Bật LED 3
  const payload = JSON.stringify({
    action: 'on',
    ledIndex: 3
  });
  client.publish('warehouse/node-A/light', payload);

  setTimeout(() => {
    // Tắt tất cả
    client.publish('warehouse/node-A/light', '{"action":"clear"}');
    client.end();
  }, 2000);
});
```

---

## 📚 Tham khảo

- **74HC595 Datasheet**: https://www.ti.com/product/SN74HC595
- **ESP32 GPIO**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html
- **PubSubClient**: https://github.com/knolleary/pubsubclient
- **ArduinoJson**: https://arduinojson.org/
- **MQTT Broker**: https://www.emqx.io/ (free public broker)
- **Slide gốc**: `esp32-74hc595-mqtt.html`

---

## ✅ Checklist trước khi demo

- [ ] Cài PlatformIO CLI
- [ ] Cấu hình config.h (Wi-Fi, MQTT, GPIO)
- [ ] Build: `pio run`
- [ ] Nối ESP32 (cổng USB)
- [ ] Upload: `pio run --target upload`
- [ ] Mở Serial Monitor: `pio device monitor`
- [ ] Thấy "Setup complete!" → OK
- [ ] Mở Wokwi simulator (nếu dùng)
- [ ] Test MQTT: publish từ MQTTX / CLI
- [ ] Nhìn LED đổi → Thành công! 🎉

---

## 🎓 Bài tập

### Cơ bản
1. Điều khiển bât/tắt từng LED bằng MQTT
2. Tắt toàn bộ LED bằng action "clear"

### Nâng cao
3. Thêm action "toggle" (bật↔tắt)
4. Thêm action "blink" (nhấp nháy với tần số tùy chọn)
5. Báo lại trạng thái hiện tại mỗi khi thay đổi

### Ứng dụng
6. Viết Python script bật/tắt theo lịch
7. Tích hợp với Home Assistant
8. App di động publish MQTT

---

**Tác giả**: HUIT Lab  
**Phiên bản**: 1.0  
**Ngày**: 2026-11-18
