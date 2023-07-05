#include <Arduino.h>
#include "EspMQTTClient.h"
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

uint32_t chipId = 0;

EspMQTTClient client(
    "DigitalAGClub",  // WiFi SSID at the maker space
    "username",       // WiFi password
    "66.253.158.154", // MQTT Broker server ip
    "DAC"             // Client name that uniquely identify your device
);

DHT dht(DHTPIN, DHTTYPE);
char payload[50];

void onConnectionEstablished()
{

  client.subscribe("purdue-dac/command", [](const String &payload)
                   { Serial.println(payload); });
}

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  for (int i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  Serial.printf("\nESP32 Chip model = %s Rev %d\n",
                ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.printf("Chip ID: %X\n", chipId);

  delay(10000); // Update 10 seconds
  float h = dht.readHumidity();
  float t = dht.readTemperature(true);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°F "));

  client.loop();
  sprintf(payload, "%f", h);
  client.publish("purdue-dac/telemetry/humidity", payload, true);
  sprintf(payload, "%f", t);
  client.publish("purdue-dac/telemetry/temperature", payload, true);
}