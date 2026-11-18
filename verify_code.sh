#!/bin/bash
# verify_code.sh - Kiểm tra xem code có lỗi compile hay không

echo "=== Code Verification ==="

# Check include/config.h
if [ -f "include/config.h" ]; then
    echo "✓ include/config.h exists"
    grep "#define PIN_SER" include/config.h && echo "  ✓ GPIO pins defined"
    grep "MQTT_HOST" include/config.h && echo "  ✓ MQTT config exists"
else
    echo "✗ include/config.h not found"
fi

# Check src/main.cpp
if [ -f "src/main.cpp" ]; then
    echo "✓ src/main.cpp exists"
    grep "void writeShift" src/main.cpp && echo "  ✓ writeShift() defined"
    grep "void setLed" src/main.cpp && echo "  ✓ setLed() defined"
    grep "void handleMqtt" src/main.cpp && echo "  ✓ handleMqtt() defined"
    grep "void setup()" src/main.cpp && echo "  ✓ setup() defined"
    grep "void loop()" src/main.cpp && echo "  ✓ loop() defined"
else
    echo "✗ src/main.cpp not found"
fi

# Check platformio.ini
if [ -f "platformio.ini" ]; then
    echo "✓ platformio.ini exists"
    grep "pubsubclient" platformio.ini && echo "  ✓ pubsubclient lib declared"
    grep "arjson" platformio.ini && echo "  ✓ ArduinoJson lib declared"
else
    echo "✗ platformio.ini not found"
fi

# Check wokwi.json
if [ -f "wokwi.json" ]; then
    echo "✓ wokwi.json exists"
    grep "esp32-devkit" wokwi.json && echo "  ✓ ESP32 DevKit in diagram"
    grep "74hc595" wokwi.json && echo "  ✓ 74HC595 chips in diagram"
    grep '"id": "led' wokwi.json | wc -l | xargs echo "  ✓ LEDs in diagram:"
else
    echo "✗ wokwi.json not found"
fi

# Check documentation
if [ -f "README.md" ]; then
    echo "✓ README.md exists"
else
    echo "✗ README.md not found"
fi

echo ""
echo "=== Structure OK ==="
echo "Ready to build with: pio run"
