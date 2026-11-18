# Code Structure Explanation — Hướng dẫn chi tiết cấu trúc code

## 📐 Sơ đồ kiến trúc

```
┌─────────────────────────────────────────────────────────────┐
│                     MAIN LOOP (loop())                      │
│                                                             │
│  1. mqtt.loop()  ← Giữ MQTT alive, nhận tin từ broker      │
│  2. Nếu có tin → Trigger handleMqtt()                      │
│  3. parseJson() → Xác định action                          │
│  4. Rẽ nhánh: on/off/set/setMany/clear                     │
│  5. writeShift() → Ghi vào 74HC595                         │
│  6. LED đổi trạng thái ngay lập tức                        │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔧 Phần 1: Điều khiển Shift Register (GPIO)

### Hàm: `writeShift(uint16_t mask)`

```cpp
void writeShift(uint16_t mask) {
  digitalWrite(PIN_RCLK, LOW);                           // ① Hạ Latch
  shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST, (mask >> 8) & 0xFF);  // ② Byte cao
  shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST,  mask & 0xFF);        // ③ Byte thấp
  digitalWrite(PIN_RCLK, HIGH);                          // ④ Lên Latch
}
```

**Giải thích từng bước:**

1. **`digitalWrite(PIN_RCLK, LOW)`**
   - Kéo chân Latch (STCP) xuống LOW
   - Chuẩn bị nhận dữ liệu từ shift register

2. **`shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST, ...)`**
   - Hàm Arduino built-in: đẩy 8 bit vào shift register
   - `PIN_SER`: chân Data (DS)
   - `PIN_SRCLK`: chân Clock (SHCP)
   - `MSBFIRST`: Most Significant Bit first (bit cao trước)
   - **Lần 1** (byte cao): Dành cho chip 2 (LED 8–15)
   - **Lần 2** (byte thấp): Dành cho chip 1 (LED 0–7)

3. **`digitalWrite(PIN_RCLK, HIGH)`**
   - Kéo Latch lên HIGH
   - **Trigger**: Q0–Q7 trên cả 2 chip cập nhật đồng loạt
   - LED sáng/tắt ngay lập tức

**Ví dụ:**
```cpp
writeShift(0x00FF);  // 0000_0000 | 1111_1111
// Byte cao: 0000_0000 → Chip 2: LED 8–15 tắt hết
// Byte thấp: 1111_1111 → Chip 1: LED 0–7 sáng hết
```

---

### Hàm: `setLed(uint8_t index, bool on)`

```cpp
void setLed(uint8_t index, bool on) {
  if (index >= TOTAL_LEDS) return;
  if (on) {
    ledMask |=  (1u << index);     // Set bit (sáng)
  } else {
    ledMask &= ~(1u << index);     // Clear bit (tắt)
  }
  writeShift(ledMask);             // Ghi vào chip
}
```

**Giải thích:**
- `ledMask`: biến global lưu trạng thái 16 LED
- `(1u << index)`: Tạo mask với bit index = 1
- `|=`: OR → Set bit (bật LED)
- `&= ~`: AND NOT → Clear bit (tắt LED)
- Sau mỗi thay đổi, gọi `writeShift()` để cập nhật

**Ví dụ:**
```cpp
ledMask = 0;
setLed(0, true);   // LED0 sáng → ledMask = 0x0001
setLed(5, true);   // LED5 sáng → ledMask = 0x0021
setLed(0, false);  // LED0 tắt  → ledMask = 0x0020
```

---

### Hàm: `clearAll()`

```cpp
void clearAll() {
  ledMask = 0;
  writeShift(ledMask);  // Ghi 0x0000 → Tắt hết
}
```

**Tác dụng:** Tắt tất cả 16 LED cùng lúc

---

## 🌐 Phần 2: Wi-Fi & MQTT

### Hàm: `connectWifi()`

```cpp
void connectWifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);                      // ① Chế độ Station (client)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);     // ② Bắt đầu kết nối
  
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED      // ③ Chờ kết nối
         && millis() - start < 20000) {     //    Timeout 20s
    delay(250);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWiFi failed");
  }
}
```

**Trình tự:**
1. Set Wi-Fi mode = Station (kết nối tới Access Point)
2. Gọi `WiFi.begin()` = khởi động scan + kết nối
3. Chờ tối đa 20 giây
4. In log thành công/thất bại

---

### Hàm: `connectMqtt()`

```cpp
void connectMqtt() {
  if (mqtt.connected()) return;  // Đã kết nối → thoát
  
  // ① Tạo clientId duy nhất (tránh xung đột trên broker)
  uint32_t chipId = (uint32_t)ESP.getEfuseMac();
  String clientId = String("shelf-") + DEVICE_ID + "-" + String(chipId, HEX);
  
  // ② Setup server
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(handleMqtt);  // Gắn callback xử lý tin
  
  // ③ Kết nối broker
  if (mqtt.connect(clientId.c_str())) {
    Serial.println("MQTT connected!");
    
    // ④ Subscribe topic (đăng ký nhận tin)
    String topic = "warehouse/" + String(DEVICE_ID) + "/light";
    mqtt.subscribe(topic.c_str(), 1);  // QoS = 1
    Serial.printf("Subscribed to: %s\n", topic.c_str());
    
    // ⑤ Publish status (báo "online")
    mqtt.publish("warehouse/node-A/status", "{\"online\":true}");
  } else {
    Serial.printf("MQTT connection failed, rc=%d\n", mqtt.state());
  }
}
```

**Trình tự:**
1. **Kiểm tra**: Nếu đã kết nối → thoát
2. **Tạo ID client**: Duy nhất trên broker (tránh disconnect client cũ)
3. **Setup PubSubClient**: Chỉ định broker + callback
4. **connect()**: Kết nối tới broker
5. **subscribe()**: Đăng ký nhận tin từ topic `warehouse/{id}/light`
6. **publish()**: Báo status online (tuỳ chọn)

---

### Hàm: `handleMqtt()` — Xử lý tin MQTT

```cpp
void handleMqtt(char* topic, byte* payload, unsigned int length) {
  // === BƯỚC 1: In raw payload (debug) ===
  Serial.printf("[MQTT] Topic: %s | Length: %d\n", topic, length);
  Serial.print("[MQTT] Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.write(payload[i]);
  }
  Serial.println();
  
  // === BƯỚC 2: Parse JSON ===
  JsonDocument doc;
  if (deserializeJson(doc, payload, length)) {
    Serial.println("[ERROR] JSON parse failed");
    return;  // Nếu JSON sai → bỏ qua
  }
  
  // === BƯỚC 3: Lấy action ===
  const char* action = doc["action"] | "";
  Serial.printf("[JSON] action: %s\n", action);
  
  // === BƯỚC 4: Rẽ nhánh theo action ===
  if (strcmp(action, "on") == 0) {
    int ledIndex = doc["ledIndex"] | -1;
    if (ledIndex >= 0 && ledIndex < TOTAL_LEDS) {
      setLed(ledIndex, true);
      Serial.printf("[LED] ON #%d\n", ledIndex);
    }
  }
  else if (strcmp(action, "off") == 0) {
    // ... tương tự "on"
  }
  else if (strcmp(action, "clear") == 0) {
    clearAll();
  }
  else if (strcmp(action, "set") == 0) {
    // Bitmask: ghi trực tiếp
    ledMask = doc["mask"] | 0;
    writeShift(ledMask);
    Serial.printf("[LED] SET mask: 0x%04X\n", ledMask);
  }
  else if (strcmp(action, "setMany") == 0) {
    // Danh sách kênh
    bool state = doc["state"] | true;
    JsonArray channels = doc["channels"];
    for (JsonVariant ch : channels) {
      int idx = ch.as<int>();
      if (idx >= 0 && idx < TOTAL_LEDS) {
        setLed(idx, state);
      }
    }
    Serial.printf("[LED] SETMANY state=%d\n", state);
  }
}
```

**Luồng:**
1. **In payload** ra Serial (debug)
2. **Parse JSON** → Nếu lỗi → thoát
3. **Lấy field "action"** → Xác định lệnh
4. **Rẽ nhánh** → Gọi `setLed()`/`writeShift()`/`clearAll()`
5. **In log** kết quả

---

## 🎯 Phần 3: Arduino Setup & Loop

### Hàm: `setup()`

```cpp
void setup() {
  Serial.begin(115200);  // ① Baud rate 115200 (thường dùng)
  delay(500);            // ② Chờ serial ready
  
  Serial.println("\n\n=== ESP32 + 74HC595 + MQTT Demo ===");
  
  // ③ Setup GPIO (output)
  pinMode(PIN_SER, OUTPUT);
  pinMode(PIN_SRCLK, OUTPUT);
  pinMode(PIN_RCLK, OUTPUT);
  
  // ④ Khởi tạo tắt tất cả LED
  clearAll();
  
  // ⑤ Kết nối Wi-Fi
  connectWifi();
  
  // ⑥ Setup MQTT
  mqtt.setBufferSize(512);  // JSON đủ chỗ (511 byte default)
  connectMqtt();
  
  Serial.println("Setup complete!\n");
}
```

**Trình tự:**
1. Khởi tạo Serial (in log)
2. Chờ serial ready
3. Set 3 chân GPIO = output
4. Tắt tất cả LED
5. Kết nối Wi-Fi
6. Kết nối MQTT

---

### Hàm: `loop()`

```cpp
void loop() {
  // ① Reconnect Wi-Fi nếu mất
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastReconnectAttempt > 10000) {
      lastReconnectAttempt = millis();
      connectWifi();
    }
  }
  
  // ② Reconnect MQTT nếu mất
  if (!mqtt.connected()) {
    if (millis() - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = millis();
      connectMqtt();
    }
  }
  
  // ③ Giữ MQTT alive — BẮT BUỘC —
  //    Nếu không có hàng này → không nhận tin MQTT
  mqtt.loop();
  
  delay(10);
}
```

**Chuyên mục:**
1. **Kiểm tra Wi-Fi**: Nếu mất → thử kết nối lại
2. **Kiểm tra MQTT**: Nếu mất → thử kết nối lại
3. **mqtt.loop()**: **BẮT BUỘC** — Hàm này:
   - Nhận tin từ broker
   - Trigger `handleMqtt()` callback nếu có tin
   - Gửi keepalive ping
   - **Nếu quên → không bao giờ nhận tin MQTT**

---

## 🔄 Luồng hoạt động từ đầu tới cuối

```
1. Bật ESP32
   ↓
2. setup() chạy
   ├─ Serial init
   ├─ GPIO init (DS, SHCP, STCP)
   ├─ LED tắt hết (clearAll)
   ├─ Wi-Fi connect
   └─ MQTT connect & subscribe
   ↓
3. loop() chạy liên tục
   ├─ Kiểm tra Wi-Fi sống?
   ├─ Kiểm tra MQTT sống?
   ├─ mqtt.loop()  ← Chỉnh chốt: nhận tin từ broker
   │  └─ Nếu có tin tới topic "warehouse/node-A/light"
   │     └─ Trigger handleMqtt()
   │        ├─ Parse JSON
   │        └─ Rẽ nhánh action
   │           ├─ "on"     → setLed(index, true)
   │           ├─ "off"    → setLed(index, false)
   │           ├─ "set"    → writeShift(mask)
   │           ├─ "setMany"→ setLed() × N
   │           └─ "clear"  → clearAll()
   │              └─ LED cập nhật ngay
   └─ Lặp lại 10ms
   
   
4. Từ ngoài: publish JSON
   ├─ Broker nhận
   ├─ Forward tới ESP32
   └─ ESP32 xử lý → LED đổi
```

---

## 📌 Điểm chính (Key Takeaways)

| Hàm | Tác dụng | Quan trọng |
|-----|---------|----------|
| `writeShift()` | Ghi 16-bit vào 74HC595 (GPIO level) | ⭐⭐⭐ |
| `setLed()` | Bật/tắt 1 LED (helper) | ⭐⭐ |
| `clearAll()` | Tắt hết 16 LED | ⭐ |
| `connectWifi()` | Kết nối Wi-Fi | ⭐⭐⭐ |
| `connectMqtt()` | Kết nối broker + subscribe | ⭐⭐⭐ |
| `handleMqtt()` | Xử lý tin MQTT nhận được | ⭐⭐⭐ |
| `setup()` | Khởi tạo lần 1 | ⭐⭐⭐ |
| `loop()` | Vòng lặp chính | ⭐⭐⭐ |

**⭐⭐⭐ = Phải hiểu rõ**  
**⭐⭐ = Hiểu được logic**  
**⭐ = Biết nó làm gì**

---

## 🎓 Bài tập hiểu code

1. **Nếu quên `mqtt.loop()` trong `loop()`:**
   - Sẽ sao? → Không nhận tin MQTT (LED không đổi)
   - Sửa: Thêm lại `mqtt.loop();`

2. **Muốn bật LED 0 + LED 7 cùng lúc:**
   - Dùng `setMany`: `{"action":"setMany","channels":[0,7],"state":true}`
   - Hoặc dùng `set` bitmask: `{"action":"set","mask":129}` (0x81 = bit 0 + 7)

3. **Payload JSON sai cú pháp (ví dụ: thiếu dấu `):**
   - Hàm `deserializeJson()` sẽ return true (error)
   - `handleMqtt()` sẽ in `[ERROR] JSON parse failed` → thoát
   - LED không đổi

4. **Muốn thêm action "toggle" (bật↔tắt):**
   - Thêm vào `handleMqtt()`:
   ```cpp
   else if (strcmp(action, "toggle") == 0) {
     int idx = doc["ledIndex"] | -1;
     if (idx >= 0) {
       bool current = (ledMask >> idx) & 1;
       setLed(idx, !current);  // Đảo
     }
   }
   ```

---

**Hiểu rõ 7 hàm chính này → Dễ dàng mở rộng code! 💪**
