#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "DHTesp.h"
#include <WiFiManager.h>

DHTesp dht;
#define      DHT22_PIN 21  // only the pin, not the type !

// Web server running on port 80
WebServer server(80);


const char *SSID = "7490Keller";
const char *PWD = "nohnw122";


float temperature;
float humidity;

float rain = 111;  // only placeholders for future projects 
float wind = 222;  // only placeholders for future projects
float pressure = 333; // only placeholders for future projects
unsigned long measureDelay = 3000;                //    NOT LESS THAN 2000!!!!!   
unsigned long lastTimeRan;

StaticJsonDocument<1024> jsonDocument;

char buffer[1024];

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  
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
  //Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addJsonObject("temperature", temperature, "°C");
  addJsonObject("humidity", humidity, "%");
  addJsonObject("rain", rain, "mm");
  addJsonObject("wind", wind, "m");
  addJsonObject("pressure", pressure, "hPa");
  
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
  }

  dht.setup(DHT22_PIN, DHTesp::DHT11); // change DHT11,if you use DHT22
  Serial.println(WiFi.localIP());
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
    float tempC = dht.getTemperature();
    // put here the code to get the information about Rain and Wind
    Serial.print("Humidity");
    Serial.println(humi);
    Serial.print("Temperature");
    Serial.println(tempC);
    Serial.print("Rain");
    Serial.println(rain);
    Serial.print("Wind");
    Serial.println(wind);
    Serial.print("Pressure");
    Serial.println(pressure);
    lastTimeRan = millis();

   }
}