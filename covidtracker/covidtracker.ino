
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EasyButton.h>

#include <pcf8563.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include "ttgo.h"
#include "on.h"
#include "off.h"
#include "sensor.h"
//#include "img.h"
#define TP_PIN_PIN          33
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define IMU_INT_PIN         38
#define RTC_INT_PIN         34
#define BATT_ADC_PIN        35
#define VBUS_PIN            36
#define TP_PWR_PIN          25
#define LED_PIN             4
#define CHARGE_PIN          32

const char* ssid     = "Shreeji";
const char* password = "9998328464Mm";

const size_t capacity = JSON_ARRAY_SIZE(96) + 10 * JSON_OBJECT_SIZE(6) + 86 * JSON_OBJECT_SIZE(8) + 11110;
DynamicJsonDocument doc(capacity);

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
EasyButton tp_button(33, 80, true, false);

extern MPU9250 IMU;
bool flag = 1;
bool statusofmachin = 0;
char buff[256];
bool flag0 = 0, flag1 = 0;
int onp = -1;
String publishdata;
int onpd;
bool machinflag;



void onPressed() {
  Serial.println("Button has been pressed!");
  digitalWrite(LED_PIN, 1);
  delay(100);
  digitalWrite(LED_PIN, 0);
  onp++;
  if (onpd == 1)
  {
    int i = onp;
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
    tft.setTextSize(2);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString((String)countryName,  tft.width() / 2, 10 );
    tft.setTextSize(1);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("confirmed:- " + (String)confirmed,  tft.width() / 2, 35 );
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("recovered:- " + (String)recovered,  tft.width() / 2, 50 );
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("deaths:- " + (String)deaths,  tft.width() / 2, 65 );
  }
  else
    onp = -1;

  if (onpd == 2)
  {
    onpd = 1;
    onp = -1;
  }
 if (onp == 94)
  {
    
    onp = -1;
  }
}

void onPressedForDuration() {
  onpd++;
  digitalWrite(LED_PIN, 1);
  delay(100);
  digitalWrite(LED_PIN, 0);
  Serial.println("Button has been LONGpressed!");
  if (onpd == 1)
  {
    tft.pushImage(0, 0,  160, 80, stayhome);
    delay(1000);
  }
  if (onpd == 2)
  {
    tft.setTextSize(2);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("UPDATEING",  tft.width() / 2, tft.height() / 2 );
    httpupdate();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("UPDATED",  tft.width() / 2, tft.height() / 2 );

  }
  if (onpd == 3)
  {
    tft.setTextSize(3);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("BYE BYE",  tft.width() / 2, tft.height() / 2 );
    delay(1500);
    tft.writecommand(ST7735_SLPIN);
    tft.writecommand(ST7735_DISPOFF);
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_33, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_deep_sleep_start();
  }

}

void initButton()
{
  pinMode(25, PULLUP);
  digitalWrite(25, HIGH);
  tp_button.begin();
  tp_button.onPressedFor(600, onPressedForDuration);
  tp_button.onPressed(onPressed);

}
void setup()

{
  pinMode(4, OUTPUT);
  Serial.begin(115200);
  setupMPU9250();
  initButton();
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(0, 0,  160, 80, ttgo);
  //tft.drawString("INDIA",  140, 80 );
  httpupdate();
  delay(1500);

  // createArray("/in.jpeg");
}


void loop()
{
  static long UpdateFirst = millis();
  if (millis() - UpdateFirst > 20000)
  {
    UpdateFirst = millis();
    Serial.println("5000");
    //httpupdate();
  }




  tp_button.read();

}


void httpupdate()
{

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

    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}



void IMU_Show()
{

  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  readMPU9250();
  snprintf(buff, sizeof(buff), "--  ACC  GYR   MAG");
  tft.drawString(buff, 0, 0);
  snprintf(buff, sizeof(buff), "x %.2f  %.2f  %.2f", (int)1000 * IMU.ax, IMU.gx, IMU.mx);
  tft.drawString(buff, 0, 16);
  snprintf(buff, sizeof(buff), "y %.2f  %.2f  %.2f", (int)1000 * IMU.ay, IMU.gy, IMU.my);
  tft.drawString(buff, 0, 32);
  snprintf(buff, sizeof(buff), "z %.2f  %.2f  %.2f", (int)1000 * IMU.az, IMU.gz, IMU.mz);
  tft.drawString(buff, 0, 48);
  delay(100);
}
