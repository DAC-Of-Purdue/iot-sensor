#include <Arduino.h>
#include "EspMQTTClient.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include "time.h"

#define LED 2         // On-board LED
#define DHTPIN 4      // Connection pin
#define DHTTYPE DHT11 // DHT version
#define PUMPAPOS 18   // Pump A positive pin
#define PUMPANEG 19   // Pump A negative pin
#define PUMPBPOS 22   // Pump B positive pin
#define PUMPBNEG 23   // Pump B negative pin

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 20       /* Time ESP32 will go to sleep (in minutes) */

#define HTTPURL "http://66.253.158.154:50005/groot/command" // COmmand server

const char *ntpServer = "pool.ntp.org"; // Timestamp server
char deviceName[20] = "3078-outside";   // Name that uniquely identify your device

EspMQTTClient client(
    "3031R",
    "AreUsure",
    "66.253.158.154", // MQTT Broker server ip
    "groot");

char payload[50];
bool isMeasure, isMqttSent, isWater = false;
DHT dht(DHTPIN, DHTTYPE); // Create a variable for the sensor

unsigned long epochTime; // Timestamp
float humidity, temperature;

void onConnectionEstablished();
unsigned long getTime();
void runPumps(float);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  configTime(0, 0, ntpServer); // For timestamp
  client.enableDebuggingMessages();
  client.setWifiReconnectionAttemptDelay(1000);
  esp_sleep_enable_timer_wakeup(60 * TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //  Pin setup
  pinMode(PUMPAPOS, OUTPUT);
  pinMode(PUMPANEG, OUTPUT);
  pinMode(PUMPBPOS, OUTPUT);
  pinMode(PUMPBNEG, OUTPUT);
  digitalWrite(PUMPAPOS, LOW);
  digitalWrite(PUMPANEG, LOW);
  digitalWrite(PUMPBPOS, LOW);
  digitalWrite(PUMPBNEG, LOW);
  pinMode(LED, OUTPUT);    // Initialize LED
  digitalWrite(LED, HIGH); // Set LED to off
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

  if (client.isWifiConnected() and epochTime == 0)
  {
    epochTime = getTime();
    Serial.printf("Timestamp: %ld\n", epochTime);
  }

  if (client.isMqttConnected() and epochTime != 0 and isMeasure and !isMqttSent)
  {
    sprintf(payload, "%.1f:%.1f:%d", temperature, humidity, epochTime);
    if (client.publish("purdue-dac/groot", payload, true))
    {
      isMqttSent = true;
    }
  }

  if (client.isWifiConnected() and !isWater)
  {
    isWater = true;
    HTTPClient http;
    http.begin(HTTPURL);
    int httpCode = http.GET();
    if (httpCode == 200)
    {
      // Allocate the JSON document
      // Use arduinojson.org/v6/assistant to compute the capacity.
      StaticJsonDocument<36> doc;
      DeserializationError error = deserializeJson(doc, http.getString());

      if (error)
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      }
      else
      {
        bool water = doc["water"];
        if (water)
        {
          Serial.println("Need to water");
          runPumps(0.2);

          StaticJsonDocument<16> doc;
          doc["water"] = true;
          String output;
          serializeJson(doc, output);
          http.begin(HTTPURL);
          int httpCode = http.POST(output);
        }
      }
    }
  }

  if (isMqttSent and isWater)
  {
    Serial.println("Going to sleep now");
    Serial.flush();
    esp_deep_sleep_start();
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

void runPumps(float seconds)
{
  digitalWrite(PUMPAPOS, HIGH);
  delay(seconds * 1000);
  digitalWrite(PUMPAPOS, LOW);
  delay(1000);
  digitalWrite(PUMPBPOS, HIGH);
  delay(seconds * 1000);
  digitalWrite(PUMPBPOS, LOW);
}