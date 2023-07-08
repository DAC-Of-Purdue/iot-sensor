#include <Arduino.h>
#include "EspMQTTClient.h"
#include "DHT.h"
// Define the sensor
#define DHTPIN 4      // Connection pin
#define DHTTYPE DHT11 // DHT version

uint32_t chipId = 0;
EspMQTTClient client(
    "DigitalAGClub",  // WiFi SSID at the maker space
    "username",       // WiFi password
    "66.253.158.154", // MQTT Broker server ip
    "DAC"             // Client name that uniquely identify your device
);

DHT dht(DHTPIN, DHTTYPE); // Create a variable for the sensor
char payload[50];

void onConnectionEstablished()
// This function activate when connection is established with the broker
{
  client.subscribe( // Subscribe to a command topic
      "purdue-dac/command",
      [](const String &payload)
      { Serial.println(payload); });
}

void setup()
{
  Serial.begin(9600);
  dht.begin(); // Initialize the sensor
}

void loop()
{
  for (int i = 0; i < 17; i = i + 8)
  // Read chip ID
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  // Printing controller detail
  Serial.printf(
      "\nESP32 Chip model = %s Rev %d\n",
      ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.printf("Chip ID: %X\n", chipId);
  // Read values from the sensor
  delay(10000); // Update 10 seconds
  float h = dht.readHumidity();
  float t = dht.readTemperature(true);
  // Print values to the terminal
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°F "));
  // Send values to the broker
  client.loop();
  sprintf(payload, "%f", h);
  client.publish("purdue-dac/telemetry/humidity", payload, true);
  sprintf(payload, "%f", t);
  client.publish("purdue-dac/telemetry/temperature", payload, true);
}
