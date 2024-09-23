//This is a simple test for the LGFX-Grafik and TFT-Libarys and the BME280
//Use it, before you load the main code
//so you can find errors in the libarys and IDE-settings
//Connect theBME280 to SDA 19 and SCL 19 !


#include "TAMC_GT911.h"
#include <Arduino.h>
#include "xbm.h"
#define TOUCH_SDA 19  // This is for BME 280
#define TOUCH_SCL 20  // This is for BME 280
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH 800
#define TOUCH_HEIGHT 480
TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
#define MAX_Y 480
#define MAX_X 800
#include "LGFX_Sunton_ESP32-8048S070.h"
#include <LGFX_TFT_eSPI.hpp>
static TFT_eSPI display;
static TFT_eSprite sprite(&display);
static TFT_eSprite dtusprite(&display);
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long delayTime;
//#########################################################################################
void setup() {
  Serial.begin(115200);
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  bool status;
  status = bme.begin(0x76);
  display.init();
  //tp.begin();
  display.setBrightness(150);
  sprite.createSprite(400,480);
}

void loop() {
  /*
        tp.read();
        if (tp.isTouched){
        for (int i=0; i<tp.touches; i++){
        display.drawRect(300, 300, 100, 110, 4);
        }
        }
        else {
        display.drawRect(300, 300, 100, 110, 2);
        }
     */
  display.setCursor(0, 0);
  display.setTextSize(3);
  display.print("Temperature = ");
  display.print(bme.readTemperature());
  display.println(" *C");
  display.print("Pressure = ");
  display.print(bme.readPressure() / 100.0F);
  display.println(" hPa");
  display.print("Approx. Altitude = ");
  display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  display.println(" m");
  display.print("Humidity = ");
  display.print(bme.readHumidity());
  display.println(" %");
  display.println();
  
  sprite.fillSprite(0);  // init empty sprite
  sprite.setTextSize(4);
  sprite.setTextColor(2);
  sprite.setCursor(60, 10);
  sprite.setTextColor(TFT_RED);
  sprite.drawString("Test", 30, 50);
  sprite.setTextColor(TFT_NAVY);
  sprite.drawString("Test", 200, 50);
  sprite.drawRect(0, 0, 158, 110, TFT_WHITE);
  sprite.drawRect(162, 0, 158, 110, TFT_WHITE);
  sprite.drawXBitmap(270,143,Home8050,80,50,TFT_WHITE);
  sprite.drawXBitmap(20,130,Strom70,70,70,TFT_RED);
  sprite.drawXBitmap(110,150,PfeilGerade,150,30,TFT_GREEN);
  sprite.pushSprite(200, 200);
  delay(2000);
}
