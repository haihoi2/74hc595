#!/bin/bash
# Test MQTT publish - Demo lệnh để gửi tới ESP32

BROKER="broker.emqx.io"
TOPIC="warehouse/node-A/light"

echo "=== ESP32 + 74HC595 MQTT Test Script ==="
echo "Broker: $BROKER"
echo "Topic: $TOPIC"
echo ""

# Kiểm tra mosquitto_pub
if ! command -v mosquitto_pub &> /dev/null; then
    echo "❌ mosquitto-clients not installed"
    echo "Install: brew install mosquitto (macOS) or apt-get install mosquitto-clients (Linux)"
    exit 1
fi

# Menu
while true; do
    echo ""
    echo "Choose action:"
    echo "1. Turn ON LED 0"
    echo "2. Turn OFF LED 0"
    echo "3. Clear all LED"
    echo "4. Set bitmask 255 (8 LEDs)"
    echo "5. Set many channels [0,2,4,6]"
    echo "6. Turn ON LED 7"
    echo "7. Custom JSON (manual)"
    echo "0. Exit"
    echo ""
    read -p "Choice: " choice

    case $choice in
        1)
            payload='{"action":"on","ledIndex":0}'
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        2)
            payload='{"action":"off","ledIndex":0}'
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        3)
            payload='{"action":"clear"}'
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        4)
            payload='{"action":"set","mask":255}'
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        5)
            payload='{"action":"setMany","channels":[0,2,4,6],"state":true}'
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        6)
            payload='{"action":"on","ledIndex":7}'
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        7)
            read -p "Enter JSON payload: " payload
            echo "Sending: $payload"
            mosquitto_pub -h $BROKER -t $TOPIC -m "$payload"
            ;;
        0)
            echo "Exiting..."
            exit 0
            ;;
        *)
            echo "Invalid choice"
            ;;
    esac
done
