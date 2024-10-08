#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <Wire.h>
#include "DHTesp.h"
#include <WiFiManager.h>

DHTesp dht;

WebServer server(80);


const char *SSID = "xxxxx";
const char *PWD = "xxxxx";
int WIFIsignal = 0;
int wifi_signal;
int tempC;
float temperature;
float humidity;
unsigned long measureDelay = 2000;                //    NOT LESS THAN 2000!!!!!   
unsigned long lastTimeRan;
StaticJsonDocument<1024> jsonDocument;
char buffer[1024];

#define DHT22_PIN 21

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
   // Get RGB components
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
  addJsonObject("Wlan",wifi_signal, "db");
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
  dht.setup(DHT22_PIN, DHTesp::DHT11);
  setupApi();
  
}
void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
        // measurements every measureDelay ms
  if (millis() > lastTimeRan + measureDelay)  {
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    float humi  = dht.getHumidity();
   
    Serial.print("Die IP Adresse ist : ");
    Serial.println(WiFi.localIP());
    Serial.print("Luftdruck: ");
    Serial.println(humi);
    Serial.print("Temperatur");
    Serial.println(tempC);
    
    //clear display
    
    
    lastTimeRan = millis();
    

  
  
   }

}