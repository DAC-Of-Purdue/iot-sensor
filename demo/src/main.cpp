#include <Arduino.h>
#include "EspMQTTClient.h"
#include "DHT.h"
#include "time.h"
// Define the sensor
#define DHTPIN 5      // Connection pin
#define DHTTYPE DHT22 // DHT version
#define LED 2         // On-board LED

const char *ntpServer = "pool.ntp.org"; // Timestamp server
char deviceName[20] = "ag-week-demo-1"; // Name that uniquely identify your device

unsigned long epochTime; // Timestamp
uint32_t chipId = 0;
EspMQTTClient client(
    "DigitalAGClub", // WiFi SSID at the maker space
    "AgWeekDemo",    // WiFi password
    "broker.emqx.io" // MQTT Broker server IP
);

DHT dht(DHTPIN, DHTTYPE); // Create a variable for the sensor
char payload[50];
char topic[40];

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
  Serial.begin(9600);
  dht.begin();                                        // Initialize the sensor
  sprintf(topic, "purdue-dac/sensor/%s", deviceName); // Set MQTT Topic
  pinMode(LED, OUTPUT);                               // Initialize LED
  digitalWrite(LED, LOW);                             // Set LED to off
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
    float h = dht.readHumidity();
    float t = dht.readTemperature(true);
    // Print values to the terminal
    Serial.printf("Humidity: %.1f %\n", h);
    Serial.printf("Temperature: %.1f°F\n", t);
    // Send values to the broker
    sprintf(payload, "%.1f:%.1f:%d", t, h, epochTime);
    client.publish(topic, payload, true);
    Serial.printf("Topic: %s\n", topic);
    Serial.printf("Payload: %s\n", payload);
    blinkLED(LED, 500); // Blink LED to indicate new sending
    delay(10000);       // Update 10 seconds
  }
}
