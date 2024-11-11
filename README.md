# MqttManager Library

`MqttManager` is a lightweight, flexible library for managing MQTT connections on the ESP32. It uses the `AsyncMqttClient` library to maintain asynchronous MQTT connections, handle reconnects with exponential backoff, and send messages reliably to a specified MQTT broker.

## Features
- Configurable MQTT broker server and port
- Automatic reconnection with exponential backoff
- Last Will and Testament (LWT) message for offline/online notifications
- Callback handling for connection and disconnection events
- Easy-to-use method for publishing MQTT messages

## Installation
1. Download or clone the repository.
2. Install the required dependencies in your Arduino IDE or PlatformIO environment:
   - [AsyncMqttClient](https://github.com/marvinroger/async-mqtt-client)

## Getting Started

### Prerequisites
Ensure you have the required libraries installed:
- `AsyncMqttClient` for handling MQTT connections.

### Usage Example
```cpp
#include <Arduino.h>
#include "MqttManager.h"

MqttManager mqttManager;

void setup() {
    Serial.begin(115200);

    // Set the MQTT server and port
    mqttManager.setServer("mqtt.example.com", 1883);

    // Optionally, set the Last Will and Testament (LWT) topic
    mqttManager.setLwt("device/status");

    // Connect to the MQTT broker
    mqttManager.connect();
}

void loop() {
    // Maintain the MQTT connection and handle reconnects
    mqttManager.reconnect();

    // Example of sending a message
    mqttManager.sendMessage("device/data", "Hello, MQTT!");

    delay(1000); // Adjust as needed for your application
}