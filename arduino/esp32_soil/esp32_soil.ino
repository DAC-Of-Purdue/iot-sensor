#include <Arduino.h>
#include "EspMQTTClient.h"
#include "time.h"
// Define the sensor
#define SOILPIN 34      // Connection pin
#define LED 2         // On-board LED

const char *ntpServer = "pool.ntp.org"; // Timestamp server
char deviceName[20] = "ag-week-demo-4"; // Name that uniquely identify your device

unsigned long epochTime; // Timestamp
uint32_t chipId = 0;
EspMQTTClient client(
    "Pixel7-Tam", // WiFi SSID at the maker space
    "c6c71196ae4e",    // WiFi password
    "broker.emqx.io" // MQTT Broker server IP
);

char payload[40];
char topic[50];

void onConnectionEstablished()
// This function activate when connection is established with the broker
{
  Serial.printf("Connection Established\n");
  digitalWrite(LED, HIGH); // Turn on LED
  client.subscribe(        // Subscribe to a command topic
      "purdue-dac/command",
      [](const String &payload)
      { Serial.println(payload); });
}

void setup()
{
  Serial.begin(115200);
  sprintf(topic, "purdue-dac/sensor/%s", deviceName); // Set MQTT Topic
  pinMode(LED, OUTPUT);                               // Initialize LED
  digitalWrite(LED, LOW);                           // Set LED to off
  // Printing controller detail
  for (int i = 0; i < 17; i = i + 8)
  // Read chip ID
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  Serial.printf(
      "\nESP32 Chip model = %s Rev %d\n",
      ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.printf("Chip ID: %X\n", chipId);
  client.setMqttClientName(deviceName);
  client.setWifiReconnectionAttemptDelay(5000);
  configTime(0, 0, ntpServer); // For timestamp
}

void blinkLED(uint8_t pin, uint32_t timeDelay)
{
  digitalWrite(pin, !digitalRead(pin));
  delay(timeDelay);
  digitalWrite(pin, !digitalRead(pin));
}

unsigned long getTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    return (0);
  }
  time(&now);
  return now;
}

void loop()
{
  client.loop();
  Serial.printf("-----------------------------------\n");
  // Get timestamp
  epochTime = getTime();
  Serial.printf("Timestamp: %ld\n", epochTime);
  // Check connection
  if (!client.isConnected() or epochTime == 0)
  {
    Serial.printf("WiFi Connection: %d\n", client.isWifiConnected());
    Serial.printf("MQTT Connection: %d\n", client.isMqttConnected());
    // Need to change to timer
    blinkLED(LED, 1000);
    delay(1000);
  }
  // Read values from the sensor
  else
  {
    long soilMoisture = analogRead(SOILPIN);
    
    // Print values to the terminal
    Serial.printf("Soil Moisture (raw): %d %\n", soilMoisture);
    float percent = (4096 - soilMoisture) / 4096.0 * 100;
    Serial.printf("Soil Moisture (raw): %.1f %\n", percent);
    // Send values to the broker
    sprintf(payload, "%.1f:%d", percent, epochTime);
    client.publish(topic, payload, true);
    Serial.printf("Topic: %s\n", topic);
    Serial.printf("Payload: %s\n", payload);
    blinkLED(LED, 500); // Blink LED to indicate new sending
    delay(5000);       // Update 10 seconds
  }
}
