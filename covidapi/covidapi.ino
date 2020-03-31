/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <ArduinoJson.h>
WiFiMulti wifiMulti;

const size_t capacity = JSON_ARRAY_SIZE(96) + 10 * JSON_OBJECT_SIZE(6) + 86 * JSON_OBJECT_SIZE(8) + 11110;
DynamicJsonDocument doc(capacity);

const char* ssid     = "XXXXXXXX";
const char* password = "xxxxx";

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  wifiMulti.addAP(ssid, password);

}

void loop() {
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin("https://api.coronatracker.com/v2/analytics/country"); //HTTP
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Serial.println(payload);
        deserializeJson(doc, payload);
        for (int i = 0; i <= 95; i++)
        {
          Serial.println(i);
          JsonObject covid = doc[i];
          const char* countryCode = covid["countryCode"];
          const char* countryName = covid["countryName"]; // "United States"
          float lat = covid["lat"];
          float lng = covid["lng"];
          long confirmed = covid["confirmed"];
          int deaths = covid["deaths"];
          int recovered = covid["recovered"];
          const char* dateAsOf = covid["dateAsOf"];
          Serial.println("countryCode :  " + (String)countryCode);
          Serial.println("countryName :  " + (String)countryName);
          Serial.println("confirmed   :  " + (String)confirmed);
          Serial.println("recovered   :  " + (String)recovered);
          Serial.println("deaths      :  " + (String)deaths);
          Serial.println("dateAsOf    :  " + (String)dateAsOf);
          Serial.println("---------------------------");
          delay(50);
        }
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }

  delay(10000);
}
