#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "../include/config.h"

// ============ Global Variables ============
uint16_t ledMask = 0;              // 16-bit bitmask cho tất cả LED
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
unsigned long lastReconnectAttempt = 0;

// ============ 1. Hàm điều khiển Shift Register ============

/**
 * Ghi 16-bit vào 2 chip 74HC595 (daisy-chain)
 * MSB first: byte cao (chip 2) -> byte thấp (chip 1)
 */
void writeShift(uint16_t mask) {
  digitalWrite(PIN_RCLK, LOW);                           // Hạ Latch trước
  shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST, (mask >> 8) & 0xFF);  // Chip 2 (Q8-Q15)
  shiftOut(PIN_SER, PIN_SRCLK, MSBFIRST,  mask & 0xFF);        // Chip 1 (Q0-Q7)
  digitalWrite(PIN_RCLK, HIGH);                          // Latch lên = cập nhật Q
}

/**
 * Bật/tắt 1 kênh LED (0-15)
 */
void setLed(uint8_t index, bool on) {
  if (index >= TOTAL_LEDS) return;
  if (on) {
    ledMask |=  (1u << index);     // Set bit
  } else {
    ledMask &= ~(1u << index);     // Clear bit
  }
  writeShift(ledMask);
}

/**
 * Tắt tất cả LED
 */
void clearAll() {
  ledMask = 0;
  writeShift(ledMask);
  Serial.println("[LED] Clear all");
}

// ============ 2. Hàm Wi-Fi ============

void connectWifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(250);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi connected! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWiFi failed");
  }
}

// ============ 3. Hàm MQTT ============

/**
 * Xử lý tin MQTT nhận được
 */
void handleMqtt(char* topic, byte* payload, unsigned int length) {
  Serial.printf("[MQTT] Topic: %s | Length: %d\n", topic, length);
  
  // In raw payload để debug
  Serial.print("[MQTT] Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.write(payload[i]);
  }
  Serial.println();
  
  // Parse JSON
  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, payload, length)) {
    Serial.println("[ERROR] JSON parse failed");
    return;
  }
  
  // Lấy action
  const char* action = doc["action"] | "";
  
  Serial.printf("[JSON] action: %s\n", action);
  
  // Rẽ nhánh theo action
  if (strcmp(action, "on") == 0) {
    int ledIndex = doc["ledIndex"] | -1;
    if (ledIndex >= 0 && ledIndex < TOTAL_LEDS) {
      setLed(ledIndex, true);
      Serial.printf("[LED] ON #%d\n", ledIndex);
    }
  }
  else if (strcmp(action, "off") == 0) {
    int ledIndex = doc["ledIndex"] | -1;
    if (ledIndex >= 0 && ledIndex < TOTAL_LEDS) {
      setLed(ledIndex, false);
      Serial.printf("[LED] OFF #%d\n", ledIndex);
    }
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
    // Danh sách kênh + trạng thái
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

/**
 * Kết nối đến MQTT broker và subscribe topic
 */
void connectMqtt() {
  if (mqtt.connected()) return;
  
  uint32_t chipId = (uint32_t)ESP.getEfuseMac();
  String clientId = String("shelf-") + DEVICE_ID + "-" + String(chipId, HEX);
  
  Serial.printf("MQTT Connecting... (clientId: %s)\n", clientId.c_str());
  
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(handleMqtt);
  
  if (mqtt.connect(clientId.c_str())) {
    Serial.println("MQTT connected!");
    
    // Subscribe topic lệnh
    String topic = "warehouse/" + String(DEVICE_ID) + "/light";
    mqtt.subscribe(topic.c_str(), 1);
    Serial.printf("Subscribed to: %s\n", topic.c_str());
    
    // Publish status
    mqtt.publish("warehouse/node-A/status", "{\"online\":true}");
  } else {
    Serial.printf("MQTT connection failed, rc=%d\n", mqtt.state());
  }
}

// ============ 4. Arduino Setup & Loop ============

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n\n=== ESP32 + 74HC595 + MQTT Demo ===");
  
  // Setup GPIO
  pinMode(PIN_SER, OUTPUT);
  pinMode(PIN_SRCLK, OUTPUT);
  pinMode(PIN_RCLK, OUTPUT);
  
  // Khởi tạo tắt tất cả LED
  clearAll();
  
  // Wi-Fi
  connectWifi();
  
  // MQTT
  mqtt.setBufferSize(512);
  connectMqtt();
  
  Serial.println("Setup complete!\n");
}

void loop() {
  // Reconnect Wi-Fi nếu mất
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastReconnectAttempt > 10000) {
      lastReconnectAttempt = millis();
      connectWifi();
    }
  }
  
  // Reconnect MQTT nếu mất
  if (!mqtt.connected()) {
    if (millis() - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = millis();
      connectMqtt();
    }
  }
  
  // Giữ MQTT alive - BẮT BUỘC để nhận tin
  mqtt.loop();
  
  delay(10);
}
