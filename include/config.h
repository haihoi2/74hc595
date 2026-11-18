#ifndef CONFIG_H
#define CONFIG_H

// ============ Wi-Fi Configuration ============
#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASSWORD   ""

// ============ MQTT Configuration ============
#define MQTT_HOST       "broker.emqx.io"
#define MQTT_PORT       1883
#define DEVICE_ID       "node-A"

// ============ GPIO Pins (ESP32) ============
#define PIN_SER         14    // GPIO14 = DS   (Data In)
#define PIN_SRCLK       12    // GPIO12 = SHCP (Shift Clock)
#define PIN_RCLK        13    // GPIO13 = STCP (Latch / RClock)

// ============ LED / Shift Register Configuration ============
#define NUM_CHIPS       2     // Số chip 74HC595
#define LEDS_PER_CHIP   8     // 8 chân Q mỗi chip
#define TOTAL_LEDS      (NUM_CHIPS * LEDS_PER_CHIP)  // 16 LED

#endif
