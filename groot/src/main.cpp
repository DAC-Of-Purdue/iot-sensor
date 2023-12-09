#include <Arduino.h>
#include "EspMQTTClient.h"
#include <HTTPClient.h>
#include "DHT.h"
#include "time.h"

#define LED 2         // On-board LED
#define DHTPIN 4      // Connection pin
#define DHTTYPE DHT11 // DHT version

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 1        /* Time ESP32 will go to sleep (in seconds) */

const char *ntpServer = "pool.ntp.org"; // Timestamp server
char deviceName[20] = "3078-outside";   // Name that uniquely identify your device

EspMQTTClient client(
    "3031R",
    "AreUsure",
    "66.253.158.154", // MQTT Broker server ip
    "groot");

char payload[50];
bool isMeasure = false;   // Measurement checking flag.
DHT dht(DHTPIN, DHTTYPE); // Create a variable for the sensor

unsigned long epochTime;                 // Timestamp
char serverAddress[] = "66.253.158.154"; // server address
int port = 50005;

void onConnectionEstablished();
unsigned long getTime();
float humidity, temperature;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  configTime(0, 0, ntpServer); // For timestamp
  client.enableDebuggingMessages();
  client.setWifiReconnectionAttemptDelay(1000);
  esp_sleep_enable_timer_wakeup(60 * TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop()
{
  client.loop();

  if (!isMeasure)
  {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(true);
    Serial.printf("Humidity: %.1f %\n", humidity);
    Serial.printf("Temperature: %.1f°F\n", temperature);
    isMeasure = true;
  }

  if (client.isWifiConnected() or epochTime == 0)
  {
    epochTime = getTime();
    Serial.printf("Timestamp: %ld\n", epochTime);
  }

  if (client.isMqttConnected() and epochTime != 0 and isMeasure)
  {
    sprintf(payload, "%.1f:%.1f:%d", temperature, humidity, epochTime);
    if (client.publish("purdue-dac/groot", payload, true))
    {
      Serial.println("Going to sleep now");
      Serial.flush();
      esp_deep_sleep_start();
    }
  }
  delay(2000);
}

void onConnectionEstablished()
{
  Serial.println("Connected to broker");
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