#include <Arduino.h>
#include "EspMQTTClient.h"

#define LED 2          // On-board LED
#define SOIL_SENSOR 36 // Pin for soil sensor

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 15       /* Time ESP32 will go to sleep (in seconds) */

EspMQTTClient client(
    "3031R",
    "AreUsure",
    "66.253.158.154", // MQTT Broker server ip
    "groot");

char payload[50];
bool isMeasure = false; // Measurement checking flag

void onConnectionEstablished();
void readSensors(int);

struct measurement_t
{
  u_int32_t raw;
  u_int16_t sum = 0;
};

void setup()
{
  Serial.begin(9600);
  client.enableDebuggingMessages();
  client.setWifiReconnectionAttemptDelay(1000);
  esp_sleep_enable_timer_wakeup(60 * TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop()
{
  client.loop();

  if (!isMeasure)
  {
    readSensors(5);
  }

  if (client.isMqttConnected())
  {

    if (client.publish("purdue-dac/groot", payload))
    {
      Serial.println("Going to sleep now");
      Serial.flush();
      esp_deep_sleep_start();
    }
  }
  delay(5000);
}

void onConnectionEstablished()
{
  Serial.println("Connected to broker");
}

void readSensors(int nSample)
{
  measurement_t soilMoisture;
  for (int i = 0; i < nSample; i++)
  {
    /* Soil Moisture */
    soilMoisture.raw = analogRead(SOIL_SENSOR);
    Serial.printf("Soil Moisture: %d %\n", soilMoisture.raw);
    soilMoisture.sum += soilMoisture.raw;

    delay(1000);
  }

  sprintf(payload, "%d", (soilMoisture.sum / nSample));
  isMeasure = true;
}