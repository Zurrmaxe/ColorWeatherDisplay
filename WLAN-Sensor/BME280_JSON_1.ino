#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
//#include <WiFiManager.h>
//#include <Wire.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long delayTime;


// Web server running on port 80
WebServer server(80);


const char *SSID = "xxxxxx";
const char *PWD = "xxxxxx";


float temperature;
float humidity;
float pressure;
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
  Serial.println("Get all values");
  jsonDocument.clear(); // Clear json buffer
  addJsonObject("temperature", temperature, "°C");
  addJsonObject("humidity", humidity, "%");
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
  bool status;
        status = bme.begin(0x76);
  Serial.println(WiFi.localIP());
  setupApi();
}
void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
    // measurements every measureDelay ms
  if (millis() > lastTimeRan + measureDelay)  {
    humidity = bme.readHumidity();
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 100.0F;

    float humi  = bme.readHumidity();
    float tempC =  bme.readTemperature();
    float press = bme.readPressure() / 100.0F;
    Serial.println(humi);
    Serial.println(tempC);
    Serial.println(press);
    lastTimeRan = millis();

   }
}
