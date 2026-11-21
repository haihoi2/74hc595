# 🔄 RELOAD WOKWI SIMULATOR

## Vừa cập nhật diagram.json (đơn giản hơn)

**Thay đổi:**
- ❌ Bỏ resistor inline (gây lỗi)
- ✅ Nối trực tiếp SR output → LED anode → GND
- ✅ 44 wires rõ ràng (12 control + 32 LED)

---

## Cách reload:

### Bước 1: Stop
```
Ctrl+Shift+P → "Wokwi: Stop Simulator"
```

### Bước 2: Start
```
Ctrl+Shift+P → "Wokwi: Start Simulator"
Chờ ~30 giây...
```

### Bước 3: Verify
Bạn sẽ thấy:
- ✅ ESP32 (xanh)
- ✅ 2×74HC595 (đen)
- ✅ 16 LED (màu sắc)
- ✅ **44 dây nối rõ ràng!**

Màu dây:
- 🔵 Blue: Data
- 🟡 Yellow: Clock
- 🟠 Orange: Latch
- 🟢 Green: Cascade
- 🔴 Red: Power/LED
- ⚫ Black: GND

---

## Diagram structure:

```
ESP32
  │
  ├─→ SR1 (74HC595)
  │    ├─→ LED0, LED1, ... LED7
  │    └─→ SR2 (cascade)
  │
  ├─→ SR2 (74HC595)
  │    └─→ LED8, LED9, ... LED15
  │
  └─→ GND (all LED cathodes)
```

---

## Test ngay sau reload:

```bash
python3 << 'TESTEOF'
import paho.mqtt.client as mqtt
import json
import time

client = mqtt.Client()
client.connect("broker.emqx.io", 1883)
client.loop_start()

time.sleep(1)

# ON: LED 1, 3, 5
payload = {"action": "setMany", "channels": [1, 3, 5], "state": True}
client.publish("warehouse/node-A/light", json.dumps(payload))

time.sleep(1)
client.disconnect()
TESTEOF
```

**Expected**: LED1, LED3, LED5 sáng trên Wokwi! 🌟

---

## Nếu vẫn không thấy dây:

1. Check diagram.json file exists:
   ```bash
   ls -l diagram.json
   ```

2. Check wokwi.toml points to firmware:
   ```bash
   cat wokwi.toml
   ```

3. Check VS Code extension:
   - Ctrl+Shift+X → Search "Wokwi"
   - Reinstall if needed

4. Force reload:
   - Close VS Code
   - Reopen workspace
   - Ctrl+Shift+P → "Wokwi: Start Simulator"

---

**Status**: 📝 diagram.json updated, ready to reload!
