#include <Arduino.h>
#include <WiFiManager.h>

#include "ConfigManager.h"
#include "MqttManager.h"


WiFiManager wifiManager;
MqttManager mqttManager;

// MQTT server details as mutable variables
char mqtt_server[16] = "192.168.1.113"; // Replace with your MQTT server IP
int mqtt_port = 1883;                   // MQTT port (1883 is the default)

// MQTT topics as mutable variables
char sound_state_mqtt_topic[64] = "korngva/sound_monitor/first_floor/sound_state"; // Topic for main MCB status

// LWT (Last Will and Testament) settings as mutable variables
char lwt_topic[64] = "korngva/sound_monitor/device_status"; // LWT topic to publish on disconnect

void setup()
{

  Serial.begin(115200);
  delay(1000); // Safety


//-------wifi connection--------//
  bool is_wifi_connected;

  is_wifi_connected = wifiManager.autoConnect();
  // if wifi fail
  if (!is_wifi_connected)
  {
    Serial.println("Failed to connect");
    // ESP will reboot
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("Wifi connected... :)");
    // Set up the MQTT server and LWT topic
    mqttManager.setServer(mqtt_server, mqtt_port);   // Set the MQTT server IP and port
    mqttManager.setLwt(lwt_topic); // Set LWT topic

    // Connect to the MQTT broker
    mqttManager.connect();
    
  }
}

void loop()
{
    // Keep the MQTT connection alive
  delay(1000);  // Or you can use a proper MQTT loop mechanism

  // Optionally, send a test message every 10 seconds
  static unsigned long lastMessageTime = 0;
  if (millis() - lastMessageTime >= 10000) {
    mqttManager.sendMessage("korngva/sound_monitor/test_topic", "Hello from ESP32!");
    lastMessageTime = millis();
  }
  Serial.println("This can run while mqtt trying to connect" );
}
