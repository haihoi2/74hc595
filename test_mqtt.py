"""
Python MQTT Test Client - Điều khiển LED qua MQTT
Cài đặt: pip install paho-mqtt
"""

import paho.mqtt.client as mqtt
import json
import time

# Cấu hình
BROKER = "broker.emqx.io"
PORT = 1883
TOPIC = "warehouse/node-A/light"

# Callback
def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("✓ Connected to broker")
    else:
        print(f"✗ Connection failed with code {reason_code}")

def on_disconnect(client, userdata, flags, reason_code, properties):
    if reason_code != 0:
        print("✗ Unexpected disconnection")

# Tạo client
client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_disconnect = on_disconnect

print("=== ESP32 + 74HC595 MQTT Test (Python) ===")
print(f"Broker: {BROKER}")
print(f"Topic: {TOPIC}\n")

# Kết nối
print("Connecting...")
client.connect(BROKER, PORT, 60)
client.loop_start()

# Chờ kết nối
time.sleep(2)

# Menu
while True:
    print("\n--- Actions ---")
    print("1. Turn ON LED 0")
    print("2. Turn OFF LED 0")
    print("3. Clear all")
    print("4. Bitmask 85 (LED 0,2,4,6)")
    print("5. SetMany [1,5,10]")
    print("6. Custom")
    print("0. Exit")
    
    choice = input("Choice: ").strip()
    
    if choice == "1":
        payload = {"action": "on", "ledIndex": 0}
        print(f"→ {json.dumps(payload)}")
        client.publish(TOPIC, json.dumps(payload))
    
    elif choice == "2":
        payload = {"action": "off", "ledIndex": 0}
        print(f"→ {json.dumps(payload)}")
        client.publish(TOPIC, json.dumps(payload))
    
    elif choice == "3":
        payload = {"action": "clear"}
        print(f"→ {json.dumps(payload)}")
        client.publish(TOPIC, json.dumps(payload))
    
    elif choice == "4":
        payload = {"action": "set", "mask": 85}
        print(f"→ {json.dumps(payload)}")
        client.publish(TOPIC, json.dumps(payload))
    
    elif choice == "5":
        payload = {"action": "setMany", "channels": [1, 5, 10], "state": True}
        print(f"→ {json.dumps(payload)}")
        client.publish(TOPIC, json.dumps(payload))
    
    elif choice == "6":
        custom = input("Enter JSON: ").strip()
        try:
            json.loads(custom)  # Validate
            print(f"→ {custom}")
            client.publish(TOPIC, custom)
        except:
            print("Invalid JSON")
    
    elif choice == "0":
        break
    
    else:
        print("Invalid choice")
    
    time.sleep(0.5)

client.loop_stop()
print("Disconnected")
