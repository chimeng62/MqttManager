#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <Arduino.h>
#include <AsyncMqttClient.h>

class MqttManager {
public:
    MqttManager(); // Constructor to initialize default values
    void setServer(const char *server, int port); // Set MQTT server and port
    void setLwt(const char* topic); // Set LWT topic
    void connect(); // Connect to the MQTT broker
    void reconnect(); // Reconnect to the MQTT broker with exponential backoff
    void onConnect(AsyncMqttClient* client, bool sessionPresent); // Connection callback
    void onDisconnect(AsyncMqttClient* client, AsyncMqttClientDisconnectReason reason); // Disconnection callback
    void sendMessage(const char *topic, const char *message); // Publish a message
    bool isConnected(); // Check if the client is connected to the MQTT broker

private:
    char mqtt_server[16]; // MQTT server IP
    int mqtt_port; // MQTT port
    AsyncMqttClient mqttClient; // MQTT client instance
    char lwt_topic[64]; // LWT topic
    char offline_message[20] ="off"; // Message when ESP32 goes offline
    char online_message[20] ="on"; // Message when ESP32 comes back online
    unsigned long lastReconnectAttempt; // Time of the last reconnect attempt
    unsigned long reconnectDelay; // The delay before the next reconnection attempt
    const unsigned long maxReconnectDelay = 32000; // Maximum delay (32 seconds)
};

#endif // MQTTMANAGER_H
