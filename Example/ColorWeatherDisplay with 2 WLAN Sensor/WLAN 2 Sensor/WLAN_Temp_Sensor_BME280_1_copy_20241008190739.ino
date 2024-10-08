#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Wire.h>
#include "DHTesp.h"
#include <WiFiManager.h>
#include <Adafruit_SSD1306.h>
//oled-Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Instanziierung
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long delayTime;

WebServer server(80);

const char *SSID = "xxxx";
const char *PWD = "xxxxx";
int WIFIsignal = 0;
int wifi_signal;
int analogValue;
float temperature;
float humidity;
float pressure;
float light;
float lux;
unsigned long measureDelay = 2000;                //    NOT LESS THAN 2000!!!!!   
unsigned long lastTimeRan;
StaticJsonDocument<1024> jsonDocument;
char buffer[1024];

//for led

bool led1Status = false;
bool led2Status = false;
bool buttonstate;
bool button;
bool Signal ;

#define ledcSetup
#define ledcAttachPin
// set the pins
const int led1Pin16 = 16;  // 16 corresponds to GPIO16
const int led2Pin17 = 17;  // 17 corresponds to GPIO17

const int BUTTON = 18; 
#define ONBOARD_LED 2
#define LIGHT_SENSOR_PIN 36  // LDR_LED 2 


void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
   // Get RGB components
  led1Status = jsonDocument["led1Status"];
  led2Status = jsonDocument["led2Status"];
  button = jsonDocument["button"];
  // Respond to the client
  server.send(200, "application/json", "{}");
}

void createJson(char *name, float value, char *unit) {  
  jsonDocument.clear();
  jsonDocument["name"] = name;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}
 
void addJsonObject(char *name, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["name"] = name;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getValues() {

  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addJsonObject("temperature", temperature, "°C");
  addJsonObject("humidity", humidity, "%");
  addJsonObject("pressure", pressure, "hPa");
  addJsonObject("Wlan",wifi_signal, "db");
  addJsonObject("Light",analogRead(LIGHT_SENSOR_PIN), "LUX");
  addJsonObject("RAIN", Signal, "Status");  // only  1 or 0 
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}
void setupApi() {
  server.on("/getValues", getValues);
  server.on("/setStatus", HTTP_POST, handlePost);
 
  // start server
  server.begin();
}
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 
  
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi");
    delay(500);
    wifi_signal = WiFi.RSSI();
  }
  bool status;
  status = bme.begin(0x76);
  setupApi();
  pinMode(BUTTON,INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	pinMode(led1Pin16, OUTPUT);
  pinMode(led2Pin17, OUTPUT);
  pinMode(ONBOARD_LED, OUTPUT);
  analogSetAttenuation(ADC_11db);
}
void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
        // measurements every measureDelay ms
  if (millis() > lastTimeRan + measureDelay)  {
    int analogValue = analogRead(LIGHT_SENSOR_PIN); 
    int lux = analogRead(LIGHT_SENSOR_PIN); 
    humidity = bme.readHumidity();
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 100.0F;

    float humi  = bme.readHumidity();
    float tempC = bme.readTemperature();
    float press = bme.readPressure() / 100.0F;
    float light = (analogValue -1) ;

    Serial.print("Die IP Adresse ist : ");
    Serial.println(WiFi.localIP());
    Serial.print("Luftdruck: ");
    Serial.println(humi);
    Serial.print("Temperatur");
    Serial.println(tempC);
    Serial.print("LICHTSTÄRKE = ");
    Serial.println(analogRead(LIGHT_SENSOR_PIN));
    //clear display
    display.clearDisplay();
    display.setTextColor(WHITE); 
    display.setTextSize(1);
	  display.setCursor(0, 0); 
    display.print("Tp.=");
    display.print(tempC);
    display.print(" %= ");
    display.println(humi);
    display.print("IP :");
    display.println(WiFi.localIP());
    display.print("WLAN");
    display.print(wifi_signal);
    display.print("  LUX= ");
    display.println(analogRead(LIGHT_SENSOR_PIN));
    display.print("Rain= ");
    display.print(Signal);
    display.print(" hPa=");
    display.println(press); 
    display.display();
    lastTimeRan = millis();
    bool buttonstate =digitalRead(BUTTON);

  if (buttonstate==LOW){
    digitalWrite(ONBOARD_LED, HIGH);   // switch LED on  (LOW-active)
    Signal = true ;
    //datapostB();
    digitalWrite(led2Pin17, HIGH);
    digitalWrite(led1Pin16, HIGH);
    Serial.println("LED an");
  }else{
    digitalWrite(ONBOARD_LED, LOW);  // switch LED off (LOW-active)
    Signal = false ;
    //datapostA();
    digitalWrite(ONBOARD_LED, LOW);
    digitalWrite(led2Pin17, LOW);
    digitalWrite(led1Pin16, LOW);
    Serial.println("LED aus");
  }
   }
/*
   if(led1Status) {
    digitalWrite(led1Pin16, HIGH);
  } else {
    digitalWrite(led1Pin16, LOW);
  } 
  
  if(led2Status) {
    digitalWrite(led2Pin17, HIGH);
  } else {
    digitalWrite(led2Pin17, LOW);
  } 
  */
}