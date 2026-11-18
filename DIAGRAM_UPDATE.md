# ✅ DIAGRAM UPDATE — Nối chân chính xác

## 🔧 Vừa sửa

### Trước (không hiển thị dây):
- ❌ LED không có resistor
- ❌ Thiếu pin MR (master reset)
- ❌ Connections không đầy đủ

### Sau (sửa xong):
✅ **14 control wires** (ESP32 → SR1/SR2):
```
Data:        ESP32:14 → SR1:14 → SR1:9 → SR2:14
Clock:       ESP32:12 → SR1:11 → SR2:11 (parallel)
Latch:       ESP32:13 → SR1:12 → SR2:12 (parallel)
Power SR1:   3.3V→16, GND→8, GND→13, 3.3V→10
Power SR2:   3.3V→16, GND→8, GND→13, 3.3V→10
```

✅ **48 LED wires** (SR1/SR2 → LED + resistor):
```
Per LED:
- SR output → 220Ω resistor (1)
- Resistor → LED anode (2)
- LED cathode → GND (3)
```

✅ **1 total: 62 wires** hiển thị rõ

---

## 🚀 Cách chạy

### 1️⃣ Reload Wokwi
```bash
# VS Code
Ctrl+Shift+P → "Wokwi: Restart Simulator"
```

### 2️⃣ Thấy được:
- ✅ ESP32 (xanh)
- ✅ 2×74HC595 (đen)
- ✅ 16 LED (màu sắc)
- ✅ 16 resistor (vàng/nâu)
- ✅ **62 dây nối** (xanh, vàng, cam, đỏ, đen, xanh dương)

### 3️⃣ Test
```bash
# Terminal:
mosquitto_pub -h broker.emqx.io -t warehouse/node-A/light \
  -m '{"action":"set","mask":255}'

# Wokwi:
👀 8 LED đầu (LED0-7) sáng lên! 🌟
```

---

## 📋 Diagram Structure

```
diagram.json
├── parts: 35
│   ├── ESP32 (1)
│   ├── SR1 (74HC595)
│   ├── SR2 (74HC595)
│   ├── LED (16)
│   └── Resistor 220Ω (16)
│
└── connections: 62
    ├── Control (14)
    │   ├── Data: 1 wire
    │   ├── Clock: 2 wires (parallel)
    │   ├── Latch: 2 wires (parallel)
    │   ├── Cascade: 1 wire
    │   └── Power: 8 wires
    │
    └── LED (48)
        ├── SR1→LED0-7: 24 wires
        └── SR2→LED8-15: 24 wires
```

---

## ✨ Files Updated

- ✅ **diagram.json** (192 KB → 32 KB, optimized)
- ✅ **wokwi.json** (backup copy)
- ✅ **wokwi.toml** (config pointing to firmware.elf)

---

## 🎯 Key Connections (verify these!)

**MUST have:**

| From | To | Function | Color |
|------|----|-----------|----|
| ESP32:14 | SR1:14 | Data | Blue |
| ESP32:12 | SR1:11 | Clock | Yellow |
| ESP32:13 | SR1:12 | Latch | Orange |
| SR1:9 | SR2:14 | Cascade | Green |
| SR1:15 | R0:1 | LED0 | Red |
| R0:2 | LED0:a | LED0 resistor | Red |
| LED0:c | ESP32:GND | LED0 cathode | Black |
| ... (repeat for LED1-15) | ... | ... | ... |

---

## 🔍 Troubleshoot

| Issue | Check |
|-------|-------|
| No wires showing | 1. Reload Wokwi (Ctrl+Shift+P) <br> 2. Check diagram.json exists <br> 3. Check wokwi.toml points to firmware.elf |
| Only SR1 LEDs work | Check SR2:11 & SR2:12 connected to ESP32:12 & ESP32:13 |
| LEDs too dim | Check resistor value (should be 220Ω) |
| Random LED blink | Check clock/latch pins (may be swapped) |

---

**Version**: 2.0 (fixed)  
**Files**: diagram.json, wokwi.json, wokwi.toml  
**Status**: ✅ Ready to use
