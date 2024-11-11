#include "MqttManager.h"
#include <AsyncMqttClient.h>
/*
 * MqttManager Usage Example
 * ===========================
 * This is an example usage of the MqttManager class for managing MQTT connections.
 * The class handles connecting to the MQTT broker, sending messages, and reconnecting
 * with exponential backoff when the connection is lost.
 *
 * 1. Include the MqttManager.h header file in your main application.
 * 2. Instantiate an object of the MqttManager class in your main application.
 * 3. Call the `setServer()` function to specify the MQTT server and port.
 * 4. Optionally, set a Last Will and Testament (LWT) topic using `setLwt()`.
 * 5. Call the `connect()` function to initiate the MQTT connection.
 * 6. Use the `sendMessage()` function to send messages to the MQTT broker.
 *
 * Example:
 * ---------
 * #include "MqttManager.h"
 *
 * MqttManager mqttManager;
 *
 * void setup() {
 *     // Initialize serial communication
 *     Serial.begin(115200);
 *
 *     // Set the MQTT server and port
 *     mqttManager.setServer("mqtt.example.com", 1883);
 *
 *     // Optionally, set the LWT topic
 *     mqttManager.setLwt("my/topic/lwt");
 *
 *     // Connect to the MQTT broker
 *     mqttManager.connect();
 * }
 *
 * void loop() {
 *     // Handle MQTT reconnects and ensure the connection is maintained
 *     mqttManager.reconnect();
 *
 *     // Send a message to an MQTT topic
 *     mqttManager.sendMessage("my/topic", "Hello, MQTT!");
 *
 *     // Add other tasks or code to run in your main loop
 *     delay(1000); // Example delay
 * }
 *
 * Description of MqttManager class methods:
 * -----------------------------------------
 *
 * - `setServer(const char *server, int port)`
 *   - Sets the MQTT server (broker) address and the port to connect to.
 *   - Parameters:
 *       - `server`: The IP address or hostname of the MQTT server.
 *       - `port`: The port on which the MQTT server is listening (usually 1883).
 *
 * - `setLwt(const char *topic)`
 *   - Sets the Last Will and Testament (LWT) topic for the MQTT client.
 *   - This message is sent if the client unexpectedly disconnects from the broker.
 *   - Parameters:
 *       - `topic`: The LWT topic to use for the Last Will message.
 *
 * - `connect()`
 *   - Initiates the connection to the MQTT server.
 *   - Automatically tries to reconnect if the connection is lost.
 *
 * - `reconnect()`
 *   - Checks if the MQTT client is connected and tries to reconnect if not.
 *   - Should be called periodically in the `loop()` to maintain the connection.
 *   - Implements an exponential backoff strategy to avoid spamming the broker with connection attempts.
 *
 * - `sendMessage(const char *topic, const char *message)`
 *   - Sends a message to a specified MQTT topic.
 *   - Parameters:
 *       - `topic`: The MQTT topic where the message will be published.
 *       - `message`: The message content to be sent.
 *
 * Callback Functions:
 * -------------------
 *
 * - `onConnect(AsyncMqttClient* client, bool sessionPresent)`
 *   - Called when the client successfully connects to the MQTT broker.
 *   - Use this callback to handle post-connection tasks, like subscribing to topics.
 *
 * - `onDisconnect(AsyncMqttClient* client, AsyncMqttClientDisconnectReason reason)`
 *   - Called when the client disconnects from the MQTT broker.
 *   - Use this callback to handle disconnection events and start reconnection attempts.
 *
 * Exponential Backoff for Reconnection:
 * --------------------------------------
 * The `reconnect()` function uses an exponential backoff strategy to manage reconnection attempts:
 * - The initial reconnect delay is 1 second.
 * - The delay doubles after each failed attempt, with a maximum limit.
 * - The delay is reset to 1 second when a successful connection is established.
 *
 * This ensures the ESP32 does not overwhelm the broker with frequent connection attempts.
 *
 * Notes:
 * ------
 * - Ensure you are using an MQTT broker that supports the LWT feature for the best results.
 * - The MQTT client operates asynchronously, meaning the program will continue running
 *   even if the connection is lost, as long as `reconnect()` is called periodically.
 *
 * For more advanced MQTT features (like subscribing to topics or handling incoming messages),
 * you can expand the MqttManager class or use the built-in callback functions provided by
 * the `AsyncMqttClient` library.
 */

MqttManager::MqttManager()
    : mqtt_port(1883), // Default MQTT port
      reconnectDelay(1000), // Start with 1 second delay
      lastReconnectAttempt(0) // Start with no reconnect attempts
{
    mqttClient.onConnect([this](bool sessionPresent) {
        onConnect(&mqttClient, sessionPresent); // Call the connection callback
    });

    mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
        onDisconnect(&mqttClient, reason); // Call the disconnection callback
    });
}

// Set the MQTT server and port
void MqttManager::setServer(const char *server, int port) {
    strncpy(mqtt_server, server, sizeof(mqtt_server));
    mqtt_port = port;
}

// Set the LWT (Last Will and Testament) topic
void MqttManager::setLwt(const char* topic) {
    strncpy(lwt_topic, topic, sizeof(lwt_topic));
}

// Connect to the MQTT broker
void MqttManager::connect() {
    if (!mqttClient.connected()) {
        Serial.println("Connecting to MQTT server...");
        mqttClient.setServer(mqtt_server, mqtt_port); // Set server and port
        mqttClient.setKeepAlive(60); // Set the keep-alive interval (60 seconds)
        
        // Set LWT message (offline message)
        mqttClient.setWill(lwt_topic, 0, true, offline_message); 
        
        mqttClient.connect(); // Start the connection
    }
}

// Reconnect to the MQTT broker with exponential backoff
void MqttManager::reconnect() {
    // Attempt to reconnect if not already connected
    if (!mqttClient.connected() && millis() - lastReconnectAttempt >= reconnectDelay) {
        Serial.println("Attempting MQTT reconnect...");
        connect(); // Try to reconnect

        // Exponential backoff logic
        lastReconnectAttempt = millis();
        if (reconnectDelay < maxReconnectDelay) {
            reconnectDelay *= 2; // Double the delay after each failed attempt
        }
    }
}

// Handle connection success
void MqttManager::onConnect(AsyncMqttClient* client, bool sessionPresent) {
    Serial.println("Connected to MQTT broker");

    // Send online message when successfully connected
    sendMessage(lwt_topic, online_message); 

    reconnectDelay = 1000; // Reset the backoff delay to 1 second
}


// Handle disconnection
void MqttManager::onDisconnect(AsyncMqttClient* client, AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT broker");
    reconnect(); // Start reconnect process
}

// Send a message to a specific MQTT topic
void MqttManager::sendMessage(const char *topic, const char *message) {
    if (mqttClient.connected()) {
        mqttClient.publish(topic, 0, true, message); // Publish the message
        Serial.print("MQTT message sent: ");
        Serial.print(topic);
        Serial.print(" -> ");
        Serial.println(message);
    } else {
        Serial.println("MQTT not connected!");
        reconnect(); // Try to reconnect if disconnected
    }
}

// This method returns whether the MQTT client is connected
bool MqttManager::isConnected() {
    return mqttClient.connected(); // Return the connection status of the MQTT client
}