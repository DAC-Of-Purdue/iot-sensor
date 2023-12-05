#include <Arduino.h>
#include "EspMQTTClient.h"

#define uS_TO_S_FACTOR 1000000    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  15         /* Time ESP32 will go to sleep (in seconds) */

EspMQTTClient client(
  "3031R",
  "AreUsure",
  "66.253.158.154",  // MQTT Broker server ip
  "groot"
);

void onConnectionEstablished();

void setup() {
  Serial.begin(9600);
  client.enableDebuggingMessages();
  client.setWifiReconnectionAttemptDelay(1000);
  esp_sleep_enable_timer_wakeup(60 * TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
  while (!client.isConnected()) {
    Serial.printf("WiFi Connection: %d\n", client.isWifiConnected());
    Serial.printf("MQTT Connection: %d\n", client.isMqttConnected());
    delay(5000);
    client.loop();
  }
  delay(5000);
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void onConnectionEstablished()
{
  Serial.println("Connected to broker");
}
