# 📋 Chi tiết nối chân 74HC595 + ESP32 + 16 LED

## 🎯 Sơ đồ nối chân chính xác

### 74HC595 (Shift Register) - DIP-16

```
         ┌──────────┐
     Q1  │1       16│  VCC (3.3V)
     Q2  │2       15│  Q0
     Q3  │3       14│  DS (Serial In)
     Q4  │4   595 13│  OE (Output Enable) → GND
     Q5  │5       12│  STCP (Latch/Storage)
     Q6  │6       11│  SHCP (Clock/Shift)
     Q7  │7       10│  MR (Master Reset) → VCC
     GND │8        9│  Q7' (Serial Out → SR2:DS)
         └──────────┘
```

**Giải thích:**
- Q0-Q7: 8 output channels (nối LED)
- Q7' (pin 9): Cascade out → kết nối SR2 DS (pin 14)
- DS (pin 14): Serial input ← ESP32:14
- SHCP (pin 11): Shift clock ← ESP32:12
- STCP (pin 12): Storage/Latch clock ← ESP32:13
- OE (pin 13): Output enable → GND (always enabled)
- MR (pin 10): Master Reset → VCC (never reset)
- VCC (pin 16): +3.3V
- GND (pin 8): Ground

---

## 🔗 ESP32 → 74HC595 Control Lines

**GPIO mapping (từ config.h):**

```cpp
#define PIN_SER   14  // Data/Serial In  → SR1:14 (DS)
#define PIN_SRCLK 12  // Shift Clock     → SR1:11 (SHCP)
#define PIN_RCLK  13  // Storage Clock   → SR1:12 (STCP)
```

**Connections (3 dây):**

| ESP32 | 74HC595 (SR1) | Chức năng | Màu |
|-------|---------------|----------|------|
| GPIO14 | Pin 14 (DS) | Serial Data Input | Blue |
| GPIO12 | Pin 11 (SHCP) | Shift Register Clock | Yellow |
| GPIO13 | Pin 12 (STCP) | Storage Register Clock | Orange |
| 3.3V | Pin 16 (VCC) | Power | Red |
| GND | Pin 8 (GND) | Ground | Black |
| GND | Pin 13 (OE) | Output Enable | Black |
| 3.3V | Pin 10 (MR) | Master Reset | Red |

---

## ⛓️ Daisy-Chain (SR1 → SR2)

**SR1 output → SR2 input:**

| SR1 | SR2 | Chức năng |
|-----|-----|----------|
| Pin 9 (Q7') | Pin 14 (DS) | Cascade out → next data in |
| **Same as SR1** | Pin 11 (SHCP) | Clock signal (parallel) |
| **Same as SR1** | Pin 12 (STCP) | Latch signal (parallel) |
| 3.3V | Pin 16 (VCC) | Power |
| GND | Pin 8 (GND) | Ground |
| GND | Pin 13 (OE) | Output enable |
| 3.3V | Pin 10 (MR) | Master reset |

**Sơ đồ daisy-chain:**

```
ESP32 GPIO14 ──→ SR1:14 (DS)
              ⌙→ SR1:9 (Q7') ──→ SR2:14 (DS)

ESP32 GPIO12 ──→ SR1:11 (SHCP) ──→ SR2:11 (SHCP)
                                   (parallel)

ESP32 GPIO13 ──→ SR1:12 (STCP) ──→ SR2:12 (STCP)
                                   (parallel)
```

---

## 💡 LED Connections

**SR1 → LED 0-7:**

```
SR1:15 (Q0)  → LED0 anode ─┐
SR1:1  (Q1)  → LED1 anode ─┤
SR1:2  (Q2)  → LED2 anode ─┤
SR1:3  (Q3)  → LED3 anode ─┤
SR1:4  (Q4)  → LED4 anode ─┤  All cathodes → ESP32 GND
SR1:5  (Q5)  → LED5 anode ─┤
SR1:6  (Q6)  → LED6 anode ─┤
SR1:7  (Q7)  → LED7 anode ─┘
```

**SR2 → LED 8-15:**

```
SR2:15 (Q0)  → LED8 anode ─┐
SR2:1  (Q1)  → LED9 anode ─┤
SR2:2  (Q2)  → LED10 anode─┤
SR2:3  (Q3)  → LED11 anode─┤
SR2:4  (Q4)  → LED12 anode─┤  All cathodes → ESP32 GND
SR2:5  (Q5)  → LED13 anode─┤
SR2:6  (Q6)  → LED14 anode─┤
SR2:7  (Q7)  → LED15 anode─┘
```

---

## 🔴 Các chân quan trọng nhất

### PHẢI đúng:
1. ✅ SR1:14 ← ESP32:14 (Serial data)
2. ✅ SR1:11 ← ESP32:12 (Shift clock)
3. ✅ SR1:12 ← ESP32:13 (Latch clock)
4. ✅ SR1:9 → SR2:14 (Cascade)

### Dễ sai:
❌ **Nhầm pin 11 & 12** (clock vs latch)
❌ **Quên nối SR2:11 & SR2:12** (phải parallel từ ESP32, không phải từ SR1)
❌ **Quên GND pin 13** (output sẽ tắt)
❌ **Quên cathode LED** (LED không sáng)

---

## 🧪 Testing Checklist

```bash
# 1. Check power
- [ ] 3.3V LED sáng (trên kit)

# 2. Check connections
- [ ] Data line: ESP32:14 → SR1:14
- [ ] Clock line: ESP32:12 → SR1:11
- [ ] Latch line: ESP32:13 → SR1:12

# 3. Check cascade
- [ ] SR1:9 → SR2:14 nối đúng
- [ ] SR2 cũng có clock + latch từ ESP32

# 4. Check LED
- [ ] All LED cathode → ESP32:GND
- [ ] All LED anode → SR output

# 5. Run test
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"set","mask":255}'
  
# 6. Expected: LED 0-7 sáng (bitmask 255 = 0xFF = 0b11111111)
```

---

## 📸 Wokwi Diagram Connections (diagram.json)

**Key connections:**

```json
// ESP32 → SR1 (control lines)
["esp32:14", "sr1:14", "blue"],      // ✅ Data
["esp32:12", "sr1:11", "yellow"],    // ✅ Clock
["esp32:13", "sr1:12", "orange"],    // ✅ Latch

// SR1 → SR2 (cascade)
["sr1:9", "sr2:14", "green"],        // ✅ Q7' → DS

// ESP32 → SR1 (power)
["esp32:3V3", "sr1:16", "red"],      // ✅ VCC
["esp32:GND", "sr1:8", "black"],     // ✅ GND
["esp32:GND", "sr1:13", "black"],    // ✅ OE enable

// ESP32 → SR2 (power + control, PARALLEL to SR1)
["esp32:12", "sr2:11", "yellow"],    // ✅ Clock (PARALLEL!)
["esp32:13", "sr2:12", "orange"],    // ✅ Latch (PARALLEL!)
["esp32:3V3", "sr2:16", "red"],      // ✅ VCC
["esp32:GND", "sr2:8", "black"],     // ✅ GND
["esp32:GND", "sr2:13", "black"],    // ✅ OE enable

// SR1 → LED 0-7
["sr1:15", "led0:a"],  // Q0 → LED0
["sr1:1", "led1:a"],   // Q1 → LED1
// ... etc

// SR2 → LED 8-15
["sr2:15", "led8:a"],  // Q0 → LED8
["sr2:1", "led9:a"],   // Q1 → LED9
// ... etc

// All LED cathodes → GND
["led0:c", "esp32:GND"],
["led1:c", "esp32:GND"],
// ... etc
```

---

## ⚠️ Thường gặp lỗi

| Lỗi | Triệu chứng | Giải pháp |
|-----|-----------|----------|
| Quên OE→GND | LED không sáng | Nối SR:13 → GND |
| Quên LED cathode | LED không sáng | Nối LED:c → GND |
| Nhầm clock/latch | LED bật tắt random | Check ESP32:12→SR:11, ESP32:13→SR:12 |
| SR2 không nhận data | LED 8-15 không sáng | Check SR1:9→SR2:14 (cascade) |
| SR2 không có clock | Chỉ SR1 hoạt động | Check ESP32:12→SR2:11 & ESP32:13→SR2:12 |
| Pin 10 không nối | Thỉnh thoảng lỗi | Nối SR:10 → 3.3V (MR) |

---

## 📚 Reference

**74HC595 Datasheet pinout:**
- Pin 1-7, 15: Qx outputs (Q0, Q1-Q7)
- Pin 8: GND
- Pin 9: Q7' (cascade)
- Pin 10: MR (master reset)
- Pin 11: SHCP (shift clock)
- Pin 12: STCP (storage clock)
- Pin 13: OE (output enable)
- Pin 14: DS (serial data)
- Pin 16: VCC

**Wokwi 74HC595 pinout** (Wokwi uses standard DIP-16):
- Same as datasheet above

---

## ✅ Verification

**Test dùng binary pattern:**

```cpp
// Test 1: LED 0 sáng
writeShift(0x0001);  // 0b0000000000000001

// Test 2: LED 1 sáng
writeShift(0x0002);  // 0b0000000000000010

// Test 3: LED 0-7 sáng
writeShift(0x00FF);  // 0b0000000011111111

// Test 4: LED 8-15 sáng
writeShift(0xFF00);  // 0b1111111100000000

// Test 5: Tất cả
writeShift(0xFFFF);  // 0b1111111111111111
```

---

**Made with ❤️ for HUIT Lab**

