/* ESP32 Weather Display using an EPD 7.5" Display-Code , obtains data from Open Weather Map, decodes and then displays it.
  ####################################################################################################################################
  This Weather-Software, the ideas and concepts is Copyright (c) David Bird 2018. All rights to this software are reserved and goes to him

  Any redistribution or reproduction of any part or all of the contents in any form is prohibited other than the following:
  1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
  2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
  3. You may not, except with my express written permission, distribute or commercially exploit the content.
  4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.

  The above copyright ('as annotated') notice and this permission notice shall be included in all copies or substantial portions of the Software and where the
  software use is visible to an end-user.

  THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT. FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY
  OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  See more at http://www.dsbird.org.uk

  My work was, to convert the code to the Suntoun ESP-8048S070 Colordisplay, change the x /y values, added colors and made the code run on the new system. 
  I also wrote code to be able to display my solar system and the current values. These can be seen in the yellow area.
  The temperature sensors I use here are a DHT11, which is connected to the back of the display.

  Attention :
  Its not ready and still under construction. There is a lot of work, to make it run and stable.

  ESP_NOW did not work.

  Most difference to original weathercode is, that the ESP32 goes not to sleep. 
  The esp32 has to work all the time and get the data from the solarsystem every second.
  
  In order to update the weather data and the weather displays, a reset is triggered every 30 minutes.
*/
#include <esp_now.h>
typedef struct struct_message {
  int id;
  float temp;
  float hum;
  unsigned int readingId;
} struct_message;
int outtemp = 0 ;
int outhum = 0;
struct_message incomingReadings;
/// var DTU
#include <NTPClient.h>    // by F.Weinberg ver 3.2.1
#include <HTTPClient.h>   // by A.McEwen ver 2.2.0
const int Debug = 1 ;
#include <SPI.h>
#include "xbm.h" //my Icons
const char* aDTU = "http://192.168.XXX.XXX/api/livedata/status?inv=1382907XXXXX.json"; // for the first OpenDTU ESP32
const char* bDTU = "http://192.168.XXX.XXX/api/livedata/status?inv=1382915XXXXX.json"; // for the second OpenDTU ESP32
const char* shelly = "http://192.168.XXX.XXX/status"; //  for the Shelly 3EM
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"pool.ntp.org", 36000, 60000);
int wifiTimeOutCounter = 0;
//Touchscreen
#include "TAMC_GT911.h"
#define TOUCH_SDA 19
#define TOUCH_SCL  20
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH  800
#define TOUCH_HEIGHT 480
TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
#define MAX_Y 480
#define MAX_X 800
//Schaltuhr
int hours;
int minutes;
int seconds;
//int waittime;

//var DTU1
int atpwr = 0;
int adpwr = 0;
int areach = 0;
//var DTU2
int btpwr = 0;
int bdpwr = 0;
int breach = 0;
// var Shelly
int phase = 0;
int phasea = 0;
int phaseb= 0;
int phasec = 0;
int netz = 0;
int last = 0;
int solarleist = 0;
int einspeise = 0;

// get TFT-Libary and settings
#include "LGFX_Sunton_ESP32-8048S070.h"
#include <LGFX_TFT_eSPI.hpp>
static TFT_eSPI display;               // TFT_eSPI is an alias for LGFX.
static TFT_eSprite sprite(&display);
static TFT_eSprite dtusprite(&display);
#define SCREEN_WIDTH  800             // Set for landscape mode
#define SCREEN_HEIGHT 480

#include "owm_credentials.h"          // See 'owm_credentials' tab and enter your OWM API key and set the Wifi SSID and PASSWORD
#include <ArduinoJson.h>              // https://github.com/bblanchon/ArduinoJson needs 7 
#include <WiFi.h>                     // Built-in
#include "time.h"                   // Built-in
#include "forecast_record.h"
#include "lang_gr.h"     // Localisation (German)
enum alignment {LEFT, RIGHT, CENTER};

// special Fonts for TFT
#include "adobex11font.h"
static const lgfx::U8g2font helvB08 ( u8g2_font_helvB08_tf );
static const lgfx::U8g2font helvB10 ( u8g2_font_helvB10_tf );
static const lgfx::U8g2font helvB12 ( u8g2_font_helvB12_tf );
static const lgfx::U8g2font helvB14 ( u8g2_font_helvB14_tf );
static const lgfx::U8g2font helvB18 ( u8g2_font_helvB18_tf );
static const lgfx::U8g2font helvB24 ( u8g2_font_helvB24_tf );

boolean LargeIcon = true, SmallIcon = false;
#define Large  15           // For icon drawing, needs to be odd number for best effect
#define Small  6            //  5 For icon drawing, needs to be odd number for best effect
String  Time_str, Date_str; // strings to hold time and received weather data
int     wifi_signal, CurrentHour = 0, CurrentMin = 0, CurrentSec = 0;
long    StartTime = 0;

#define max_readings 24

Forecast_record_type  WxConditions[1];
Forecast_record_type  WxForecast[max_readings];

#include "common.h"

#define autoscale_on  true
#define autoscale_off false
#define barchart_on   true
#define barchart_off  false

float pressure_readings[max_readings]    = {0};
float temperature_readings[max_readings] = {0};
float humidity_readings[max_readings]    = {0};
float rain_readings[max_readings]        = {0};
float snow_readings[max_readings]        = {0};

long SleepDuration = 30; // Sleep time in minutes, aligned to the nearest minute boundary, so if 30 will always update at 00 or 30 past the hour
int  WakeupTime    = 6;  // Don't wakeup until after 06:00 to save battery power + restart time
int  SleepTime     = 23; // Sleep after (23+1) so is 00:00 to save battery power + restart time
#define TIME_TO_SLEEP 1ULL //1 Stunde
#define US_TO_S_FACTOR 360000000

//define DHT11
#include <DHT.h>
#define DHT_SENSOR_PIN  18 // ESP32 pin GPIO18connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//#########################################################################################
void setup() {
  StartTime = millis();
  Serial.begin(115200);
  if (StartWiFi() == WL_CONNECTED && SetupTime() == true) {
    if ((CurrentHour >= WakeupTime && CurrentHour <= SleepTime)) {
      display.init(); 
      display.setBrightness(150);
     byte Attempts = 1;
      bool RxWeather = false, RxForecast = false;
      WiFiClient client;   // wifi client object
      while ((RxWeather == false || RxForecast == false) && Attempts <= 2) { // Try up-to 2 time for Weather and Forecast data
      if (RxWeather  == false) RxWeather  = obtain_wx_data(client, "weather");
      if (RxForecast == false) RxForecast = obtain_wx_data(client, "forecast");
      Attempts++;
     }
      if (RxWeather && RxForecast) { // Only if received both Weather or Forecast proceed
      WiFi.mode(WIFI_AP_STA);
      WiFi.begin(ssid, password);
      DisplayWeather();
      delay (500);
       }
    }
        
        //Schaltuhr, Touchscreen, DTU-Sprite
        timeClient.begin();
        timeClient.setTimeOffset(7200);
        timeClient.update();
        tp.setRotation(ROTATION_NORMAL);
        tp.begin();
        dht_sensor.begin();
        sprite.createSprite(375, 257);// it is only for the yellow block 
        
         }
 }


//#########################################################################################
void loop() { 
      DisplayWeather();
      display.setBrightness(150);
      solar();
      EspReset(); // This is to reset the Display every 30 Minutes
   }

     

void callback(){
  
}


//#########################################################################################
void DisplayWeather() {   
      
  DisplayGeneralInfoSection();                 // Top line of the display
  DisplayDisplayWindSection(87, 117, WxConditions[0].Winddir, WxConditions[0].Windspeed, 65);
  DisplayMainWeatherSection(231, 80);          // Centre section of display for Location, temperature, Weather report, current Wx Symbol and wind direction
  DisplayForecastSection(0, 262);
  DisplayAstronomySection(0, 191);             // Astronomy section Sun rise/set, Moon phase and Moon icon
  //DisplayStatusSection(28, 233, wifi_signal); // Wi-Fi signal strength and Battery voltage
  DisplayStatusSection(310, 3, wifi_signal);
 
}
//#########################################################################################
void DisplayGeneralInfoSection() {
  display.setFont(&helvB08);
  display.setCursor(9,2);
  display.print(City);
  display.setCursor(75, 2);
  display.print(Date_str);
  display.setCursor(170,2);
  display.print(Time_str);
  display.setTextColor(TFT_WHITE);
}
  

//#########################################################################################
void DisplayMainWeatherSection(int x, int y) {
  DisplayConditionsSection(x + 2, y + 40, WxConditions[0].Icon, LargeIcon);
  DisplayTemperatureSection();
  DisplayPrecipitationSection(342, 115, 122, 91);
  DisplayForecastTextSection(164,199,146, 51);
  
}
//#########################################################################################
void DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int Cradius) {
  arrow(x, y, Cradius - 17, angle, 15, 27); // Show wind direction on outer circle of width and length
  display.setFont(&helvB08);
  display.drawRect(0, 0, 425, 257,TFT_GREEN);
  display.drawLine(0,15,424, 15,  TFT_GREEN);
  display.drawLine(0,30,424, 30,  TFT_GREEN);
  drawString(x, y - Cradius - 36, TXT_WIND_SPEED_DIRECTION, CENTER);
  int dxo, dyo, dxi, dyi;
  display.drawLine(0, 0, 0, y + Cradius + 30, TFT_GREEN);
  display.drawCircle(x, y, Cradius, TFT_GREEN);     // Draw compass circle
  display.drawCircle(x, y, Cradius + 1, TFT_GREEN); // Draw compass circle
  display.drawCircle(x, y, Cradius * 0.7, TFT_GREEN); // Draw compass inner circle
  for (float a = 0; a < 360; a = a + 22.5) {
    dxo = Cradius * cos((a - 90) * PI / 180);
    dyo = Cradius * sin((a - 90) * PI / 180);
    if (a == 45)  drawString(dxo + x + 10, dyo + y - 10, TXT_NE, CENTER);
    if (a == 135) drawString(dxo + x + 10, dyo + y + 5,  TXT_SE, CENTER);
    if (a == 225) drawString(dxo + x - 15, dyo + y + 5,  TXT_SW, CENTER);
    if (a == 315) drawString(dxo + x - 15, dyo + y - 10, TXT_NW, CENTER);
    dxi = dxo * 0.9;
    dyi = dyo * 0.9;
    display.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, TFT_GREEN);
    dxo = dxo * 0.7;
    dyo = dyo * 0.7;
    dxi = dxo * 0.9;
    dyi = dyo * 0.9;
    display.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, TFT_GREEN);
  }
  drawString(x, y - Cradius - 10,   TXT_N, CENTER);
  drawString(x, y + Cradius + 5,    TXT_S, CENTER);
  drawString(x - Cradius - 10, y - 3, TXT_W, CENTER);
  drawString(x + Cradius + 8,  y - 3, TXT_E, CENTER);
  drawString(x, y - 35, WindDegToDirection(angle), CENTER);
  drawString(x, y + 24, String(angle, 0), CENTER);
  display.setFont(&helvB18);
  drawString(x, y -18, String(windspeed, 1), CENTER);
  display.setFont(&helvB08);
  drawString(x, y + 10, (Units == "M" ? "m/s" : "mph"), CENTER);
}
//#########################################################################################
String WindDegToDirection(float winddirection) {
  int dir = int((winddirection / 22.5) + 0.5);
  String Ord_direction[16] = {TXT_N, TXT_NNE, TXT_NE, TXT_ENE, TXT_E, TXT_ESE, TXT_SE, TXT_SSE, TXT_S, TXT_SSW, TXT_SW, TXT_WSW, TXT_W, TXT_WNW, TXT_NW, TXT_NNW};
  return Ord_direction[(dir % 16)];
}
//#########################################################################################
void DisplayTemperatureSection() {
  display.drawRect(303, 15, 122, 100, TFT_GREEN); // temp outline
  drawString(365, 15, TXT_TEMPERATURES, CENTER);
  display.setFont(&helvB24);
  drawString(355, 43, String(WxConditions[0].Temperature, 1), CENTER); // Show current Temperature
  display.setFont(&helvB10);
  drawString(365, 80, String(WxConditions[0].High, 0) + "'C | " + String(WxConditions[0].Low, 0) + "'C",CENTER); // Show forecast high and Low
  display.setFont(&helvB10);
  drawString(385, 45, Units == "M" ? "´C" : "F", LEFT);
  }
//#########################################################################################
void DisplayForecastTextSection(int x, int y , int fwidth, int fdepth) {
  display.drawRect(x, y+5, 261, 53, TFT_GREEN); // forecast text outline
  display.setFont(&helvB14);
  String Wx_Description = WxConditions[0].Forecast0;
  int MsgWidth = 220; // Using proportional fonts, so be aware of making it too wide!
  if (Language == "DE") drawStringMaxWidth(x+10, y +15, MsgWidth, Wx_Description, CENTER); // Leave German text in original format, 28 character screen width at this font size
  else                  drawStringMaxWidth(x+10, y +15, MsgWidth, TitleCase(Wx_Description), CENTER); // 28 character screen width at this font size
  display.setFont(&helvB12);
}
//#########################################################################################
void DisplayForecastWeather(int x, int y, int index) {
  int fwidth = 100;

  x = x + fwidth * index;
  display.drawRect(x, y, fwidth - 2, 85, TFT_GREEN);
  display.drawLine(x, y + 20, x + fwidth - 3, y + 20, TFT_GREEN);
  DisplayConditionsSection(x + fwidth / 2, y + 45, WxForecast[index].Icon, SmallIcon);
  drawString(x + fwidth / 2, y + 1, String(ConvertUnixTime(WxForecast[index].Dt + WxConditions[0].Timezone).substring(0,5)), CENTER);
  display.setFont(&helvB12);
  drawString(x + fwidth / 2, y + 65, String(WxForecast[index].High, 0) + "'C/" + String(WxForecast[index].Low, 0) + "'C",CENTER);

}
//#########################################################################################

//#########################################################################################
void DisplayPrecipitationSection(int x, int y, int pwidth, int pdepth) {
  uint16_t  x1, y1; //top left corner
  x1 = x - 39; //top left corner
  y1 = y - 1;  //top left corner
  display.drawRect(x1, y1, pwidth, pdepth, TFT_GREEN); // precipitation outline
  display.setFont(&helvB08);
  drawString(x1 + pwidth / 2, y + 4, TXT_PRECIPITATION_SOON, CENTER);
  display.setFont(&helvB12);
  if (WxForecast[1].Rainfall >= 0.005) { // Ignore small amounts
    drawString(x1 + pwidth / 2 + 28, y + 30, String(WxForecast[1].Rainfall, 2) + (Units == "M" ? "mm" : "in"), RIGHT); // Only display rainfall total today if > 0
    addraindrop(x1 + pwidth / 2 + 34, y + 32, 7);
  }
  if (WxForecast[1].Snowfall >= 0.005) {  // Ignore small amounts
    drawString(x1 + pwidth / 2 + 28, y + 57, String(WxForecast[1].Snowfall, 2) + (Units == "M" ? "mm" : "in"), RIGHT); // Only display snowfall total today if > 0
    addsnow(x1 + pwidth / 2 + 55, y + 45, 2, 7, SmallIcon);
  }
  if (WxForecast[1].Pop >= 0.005)       // Ignore small amounts
    drawString(x1 + pwidth / 2, y + 67, String(WxForecast[1].Pop*100, 0) + "%", CENTER); // Only display pop if > 0
}
//#########################################################################################
void DisplayAstronomySection(int x, int y) {
  display.drawRect(x, y + 13, 165, 53, TFT_GREEN);
  display.setFont(&helvB08);
  drawString(x + 5, y + 18, ConvertUnixTime(WxConditions[0].Sunrise + WxConditions[0].Timezone).substring(0, 5) + " " + TXT_SUNRISE, LEFT);
  drawString(x + 5, y + 34, ConvertUnixTime(WxConditions[0].Sunset + WxConditions[0].Timezone).substring(0, 5) + " " + TXT_SUNSET, LEFT);
  time_t now = time(NULL);
  struct tm * now_utc  = gmtime(&now);
  const int day_utc = now_utc->tm_mday;
  const int month_utc = now_utc->tm_mon + 1;
  const int year_utc = now_utc->tm_year + 1900;
  drawString(x + 5, y + 50, MoonPhase(day_utc, month_utc, year_utc, Hemisphere), LEFT);
  DrawMoon(x + 100, y, day_utc, month_utc, year_utc, Hemisphere);
}
//#########################################################################################
void DrawMoon(int x, int y, int dd, int mm, int yy, String hemisphere) {
  const int diameter = 38;
  double Phase = NormalizedMoonPhase(dd, mm, yy);
  hemisphere.toLowerCase();
  if (hemisphere == "south") Phase = 1 - Phase;
  // Draw dark part of moon
  display.fillCircle(x + diameter - 1, y + diameter, diameter / 2 + 1, TFT_DARKGREY);
  const int number_of_lines = 90;
  for (double Ypos = 0; Ypos <= number_of_lines / 2; Ypos++) {
    double Xpos = sqrt(number_of_lines / 2 * number_of_lines / 2 - Ypos * Ypos);
    // Determine the edges of the lighted part of the moon
    double Rpos = 2 * Xpos;
    double Xpos1, Xpos2;
    if (Phase < 0.5) {
      Xpos1 = -Xpos;
      Xpos2 = Rpos - 2 * Phase * Rpos - Xpos;
    }
    else {
      Xpos1 = Xpos;
      Xpos2 = Xpos - 2 * Phase * Rpos + Rpos;
    }
    // Draw light part of moon
    double pW1x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
    double pW1y = (number_of_lines - Ypos)  / number_of_lines * diameter + y;
    double pW2x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
    double pW2y = (number_of_lines - Ypos)  / number_of_lines * diameter + y;
    double pW3x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
    double pW3y = (Ypos + number_of_lines)  / number_of_lines * diameter + y;
    double pW4x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
    double pW4y = (Ypos + number_of_lines)  / number_of_lines * diameter + y;
    display.drawLine(pW1x, pW1y, pW2x, pW2y, TFT_YELLOW);
    display.drawLine(pW3x, pW3y, pW4x, pW4y, TFT_YELLOW);
  }
  display.drawCircle(x + diameter - 1, y + diameter, diameter / 2, TFT_DARKGREY);
}
//#########################################################################################
String MoonPhase(int d, int m, int y, String hemisphere) {
  int c, e;
  double jd;
  int b;
  if (m < 3) {
    y--;
    m += 12;
  }
  ++m;
  c   = 365.25 * y;
  e   = 30.6  * m;
  jd  = c + e + d - 694039.09;     /* jd is total days elapsed */
  jd /= 29.53059;                        /* divide by the moon cycle (29.53 days) */
  b   = jd;                              /* int(jd) -> b, take integer part of jd */
  jd -= b;                               /* subtract integer part to leave fractional part of original jd */
  b   = jd * 8 + 0.5;                /* scale fraction from 0-8 and round by adding 0.5 */
  b   = b & 7;                           /* 0 and 8 are the same phase so modulo 8 for 0 */
  if (hemisphere == "south") b = 7 - b;
  if (b == 0) return TXT_MOON_NEW;              // New;              0%  illuminated
  if (b == 1) return TXT_MOON_WAXING_CRESCENT;  // Waxing crescent; 25%  illuminated
  if (b == 2) return TXT_MOON_FIRST_QUARTER;    // First quarter;   50%  illuminated
  if (b == 3) return TXT_MOON_WAXING_GIBBOUS;   // Waxing gibbous;  75%  illuminated
  if (b == 4) return TXT_MOON_FULL;             // Full;            100% illuminated
  if (b == 5) return TXT_MOON_WANING_GIBBOUS;   // Waning gibbous;  75%  illuminated
  if (b == 6) return TXT_MOON_THIRD_QUARTER;    // Third quarter;   50%  illuminated
  if (b == 7) return TXT_MOON_WANING_CRESCENT;  // Waning crescent; 25%  illuminated
  return "";
}
//#########################################################################################
void DisplayForecastSection(int x, int y) {
  display.setFont(&helvB12);
  int f = 0;
  do {
    DisplayForecastWeather(x, y, f);
    f++;
  } while (f <= 7);
  // Pre-load temporary arrays with with data - because C parses by reference
  int r = 0;
  do {
    if (Units == "I") pressure_readings[r] = WxForecast[r].Pressure * 0.02953;   else pressure_readings[r] = WxForecast[r].Pressure;
    if (Units == "I") rain_readings[r]     = WxForecast[r].Rainfall * 0.0393701; else rain_readings[r]     = WxForecast[r].Rainfall;
    if (Units == "I") snow_readings[r]     = WxForecast[r].Snowfall * 0.0393701; else snow_readings[r]     = WxForecast[r].Snowfall;
    temperature_readings[r] = WxForecast[r].Temperature;
    humidity_readings[r]    = WxForecast[r].Humidity;
    r++;
  } while (r < max_readings);
  int gwidth = 165, gheight = 87;
  int gx = (SCREEN_WIDTH - gwidth * 4) / 5 + 7;
  int gy = 365;
  int gap = gwidth + gx - 2;
  display.setFont(&helvB10);
  //drawString(SCREEN_WIDTH / 2, gy - 32, TXT_FORECAST_VALUES, CENTER); // Based on a graph height of 60
  display.setFont(&helvB10);
  // (x,y,width,height,MinValue, MaxValue, Title, Data Array, AutoScale, ChartMode)
  DrawGraph(gx + 0 * gap, gy, gwidth, gheight-2, 900, 1050, Units == "M" ? TXT_PRESSURE_HPA : TXT_PRESSURE_IN, pressure_readings, max_readings, autoscale_on, barchart_off);
  DrawGraph(gx + 1 * gap, gy, gwidth, gheight-2, 10, 30,    Units == "M" ? TXT_TEMPERATURE_C : TXT_TEMPERATURE_F, temperature_readings, max_readings, autoscale_on, barchart_off);
  DrawGraph(gx + 2 * gap, gy, gwidth, gheight-2, 0, 100,   TXT_HUMIDITY_PERCENT, humidity_readings, max_readings, autoscale_off, barchart_off);
  const int Rain_array_size = sizeof(rain_readings) / sizeof(float);
  const int Snow_array_size = sizeof(snow_readings) / sizeof(float);
  if (SumOfPrecip(rain_readings, Rain_array_size) >= SumOfPrecip(snow_readings, Snow_array_size))
    DrawGraph(gx + 3 * gap + 2, gy, gwidth, gheight-2, 0, 30, Units == "M" ? TXT_RAINFALL_MM : TXT_RAINFALL_IN, rain_readings, Rain_array_size, autoscale_on, barchart_on);
  else DrawGraph(gx + 3 * gap + 5, gy, gwidth, gheight-2, 0, 30, Units == "M" ? TXT_SNOWFALL_MM : TXT_SNOWFALL_IN, snow_readings, Snow_array_size, autoscale_on, barchart_on);
}
//#########################################################################################
void DisplayConditionsSection(int x, int y, String IconName, bool IconSize) {
  if      (IconName == "01d" || IconName == "01n")  Sunny(x, y, IconSize, IconName);
  else if (IconName == "02d" || IconName == "02n")  MostlySunny(x, y, IconSize, IconName);
  else if (IconName == "03d" || IconName == "03n")  Cloudy(x, y, IconSize, IconName);
  else if (IconName == "04d" || IconName == "04n")  MostlyCloudy(x, y, IconSize, IconName);
  else if (IconName == "09d" || IconName == "09n")  ChanceRain(x, y, IconSize, IconName);
  else if (IconName == "10d" || IconName == "10n")  Rain(x, y, IconSize, IconName);
  else if (IconName == "11d" || IconName == "11n")  Tstorms(x, y, IconSize, IconName);
  else if (IconName == "13d" || IconName == "13n")  Snow(x, y, IconSize, IconName);
  else if (IconName == "50d")                       Haze(x, y, IconSize, IconName);
  else if (IconName == "50n")                       Fog(x, y, IconSize, IconName);
  else                                              Nodata(x, y, IconSize, IconName);
  if (IconSize == LargeIcon) {
    display.drawRect(x - 69, y - 105, 140, 190, TFT_GREEN);
    display.setFont(&helvB08);
    drawString(x, y - 104, TXT_CONDITIONS, CENTER);
    display.setFont(&helvB14);
    drawString(x - 5, y + 30, String(WxConditions[0].Pressure, 0) + " HPa", CENTER);
    drawString(x - 15, y + 56, String(WxConditions[0].Humidity, 0) + "%", CENTER);
    display.setFont(&helvB10);
    drawString(x + 28, y + 59, "RH", CENTER);
    if (WxConditions[0].Visibility > 0) Visibility(x - 50, y - 78, String(WxConditions[0].Visibility) + "M");
    if (WxConditions[0].Cloudcover > 0) CloudCover(x + 28, y - 78, WxConditions[0].Cloudcover);
  }
}
//#########################################################################################
void arrow(int x, int y, int asize, float aangle, int pwidth, int plength) {
  float dx = (asize + 28) * cos((aangle - 90) * PI / 180) + x; // calculate X position
  float dy = (asize + 28) * sin((aangle - 90) * PI / 180) + y; // calculate Y position
  float x1 = 0;         float y1 = plength;
  float x2 = pwidth / 2;  float y2 = pwidth / 2;
  float x3 = -pwidth / 2; float y3 = pwidth / 2;
  float angle = aangle * PI / 180;
  float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
  float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
  float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
  float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
  float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
  float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
  display.fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, TFT_GREEN);
}
//#########################################################################################

uint8_t StartWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); // switch off AP
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  uint8_t connectionStatus;
  bool AttemptConnection = true;
  while (AttemptConnection) {
    connectionStatus = WiFi.status();
    if (millis() > start + 15000) { // Wait 15-secs maximum
      AttemptConnection = false;
    }
    if (connectionStatus == WL_CONNECTED || connectionStatus == WL_CONNECT_FAILED) {
      AttemptConnection = false;
    }
    delay(50);
  }
  if (connectionStatus == WL_CONNECTED) {
    wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
   
  }
  else Serial.println("WiFi connection *** FAILED ***");
  return connectionStatus;
}

//#########################################################################################
void DisplayStatusSection(int x, int y, int rssi) {
   display.setFont(&helvB08);
   DrawRSSI(x + 35, y + 10, rssi);
  }
//#########################################################################################
void DrawRSSI(int x, int y, int rssi) {
  int WIFIsignal = 0;
  int xpos = 1;
  for (int _rssi = -100; _rssi <= rssi; _rssi = _rssi + 20) {
    if (_rssi <= -20)  WIFIsignal = 20; //            <-20dbm displays 5-bars
    if (_rssi <= -40)  WIFIsignal = 16; //  -40dbm to  -21dbm displays 4-bars
    if (_rssi <= -60)  WIFIsignal = 12; //  -60dbm to  -41dbm displays 3-bars
    if (_rssi <= -80)  WIFIsignal = 8;  //  -80dbm to  -61dbm displays 2-bars
    if (_rssi <= -100) WIFIsignal = 4;  // -100dbm to  -81dbm displays 1-bar
    display.fillRect(x + xpos * 6, y - WIFIsignal, 5, WIFIsignal, TFT_GREEN);
    xpos++;
  }
    drawString(x + 30,  y -12, String(rssi) + " dBm", LEFT);
}

//#########################################################################################
boolean SetupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov"); //(gmtOffset_sec, daylightOffset_sec, ntpServer)
  setenv("TZ", Timezone, 1);  //setenv()adds the "TZ" variable to the environment with a value TimeZone, only used if set to 1, 0 means no change
  tzset(); // Set the TZ environment variable
  delay(100);
  bool TimeStatus = UpdateLocalTime();
  return TimeStatus;
}
//#########################################################################################
boolean UpdateLocalTime() {
  struct tm timeinfo;
  char   time_output[30], day_output[30], update_time[30];
  while (!getLocalTime(&timeinfo, 10000)) { // Wait for 10-sec for time to synchronise
    
    return false;
  }
  CurrentHour = timeinfo.tm_hour;
  CurrentMin  = timeinfo.tm_min;
  CurrentSec  = timeinfo.tm_sec;
  
  if (Units == "M") {
    if ((Language == "CZ") || (Language == "DE") || (Language == "NL") || (Language == "PL")) {
      sprintf(day_output, "%s, %02u. %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900); // day_output >> So., 23. Juni 2019 <<
    }
    else
    {
      sprintf(day_output, "%s %02u-%s-%04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
    }
    strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo);  // Creates: '14:05:49'
    sprintf(time_output, "%s %s", TXT_UPDATED, update_time);
  }
  else
  {
    strftime(day_output, sizeof(day_output), "%a %b-%d-%Y", &timeinfo); // Creates  'Sat May-31-2019'
    strftime(update_time, sizeof(update_time), "%r", &timeinfo);        // Creates: '02:05:49pm'
    sprintf(time_output, "%s %s", TXT_UPDATED, update_time);
  }
  Date_str = day_output;
  Time_str = time_output;
  return true;
}
//#########################################################################################
void DrawBattery(int x, int y) {
  
      
   display.drawXBitmap(x-10,y -14 ,rxtx,50,30,TFT_RED);
  
}
//#########################################################################################
// Symbols are drawn on a relative 10x10grid and 1 scale unit = 1 drawing unit
void addcloud(int x, int y, int scale, int linesize) {
  //Draw cloud outer
  display.fillCircle(x - scale * 3, y, scale, TFT_WHITE);                // Left most circle
  display.fillCircle(x + scale * 3, y, scale, TFT_WHITE);                // Right most circle
  display.fillCircle(x - scale, y - scale, scale * 1.4, TFT_WHITE);    // left middle upper circle
  display.fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75, TFT_WHITE); // Right middle upper circle
  display.fillRect(x - scale * 3 - 1, y - scale, scale * 6, scale * 2 + 1, TFT_WHITE); // Upper and lower lines
  //Clear cloud inner
  display.fillCircle(x - scale * 3, y, scale - linesize, TFT_WHITE);            // Clear left most circle
  display.fillCircle(x + scale * 3, y, scale - linesize, TFT_WHITE);          // Clear right most circle
  display.fillCircle(x - scale, y - scale, scale * 1.4 - linesize, TFT_WHITE);  // left middle upper circle
  display.fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75 - linesize, TFT_WHITE); // Right middle upper circle
  display.fillRect(x - scale * 3 + 2, y - scale + linesize - 1, scale * 5.9, scale * 2 - linesize * 2 + 2, TFT_WHITE); // Upper and lower lines
}
//#########################################################################################
void addraindrop(int x, int y, int scale) {
  display.fillCircle(x, y, scale / 2, TFT_GREEN);
  display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y , TFT_BLUE);
  x = x + scale * 1.6; y = y + scale / 3;
  display.fillCircle(x, y, scale / 2, TFT_GREEN);
  display.fillTriangle(x - scale / 2, y, x, y - scale * 1.2, x + scale / 2, y , TFT_NAVY);
}
//#########################################################################################
void addrain(int x, int y, int scale, bool IconSize) {
  if (IconSize == SmallIcon) scale *= 1.34;
  for (int d = 0; d < 4; d++) {
    addraindrop(x + scale * (7.8 - d * 1.95) - scale * 5.2, y + scale * 2.1 - scale / 6, scale / 1.6);
  }
}
//#########################################################################################
void addsnow(int x, int y, int nrFlakes, int scale, bool IconSize) {
  int dxo, dyo, dxi, dyi;
  for (int flakes = 0; flakes < nrFlakes; flakes++) {
    for (int i = 0; i < 360; i = i + 45) {
      dxo = 0.5 * scale * cos((i - 90) * 3.14 / 180); dxi = dxo * 0.1;
      dyo = 0.5 * scale * sin((i - 90) * 3.14 / 180); dyi = dyo * 0.1;
      display.drawLine(dxo + x + flakes * 1.5 * scale - scale * 3, dyo + y + scale * 2, dxi + x + 0 + flakes * 1.5 * scale - scale * 3, dyi + y + scale * 2, TFT_GREEN);
    }
  }
}
//#########################################################################################
void addtstorm(int x, int y, int scale) {
  y = y + scale / 2;
  for (int i = 0; i < 5; i++) {
    display.drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, TFT_GREEN);
    if (scale != Small) {
      display.drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, TFT_GREEN);
      display.drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, TFT_GREEN);
    }
    display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, TFT_GREEN);
    if (scale != Small) {
      display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, TFT_GREEN);
      display.drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, TFT_GREEN);
    }
    display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, TFT_GREEN);
    if (scale != Small) {
      display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, TFT_GREEN);
      display.drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, TFT_GREEN);
    }
  }
}
//#########################################################################################
void addsun(int x, int y, int scale, bool IconSize) {
  int linesize = 3;
  if (IconSize == SmallIcon) linesize = 1;
  display.fillRect(x - scale * 2, y, scale * 4, linesize, TFT_GREEN);
  display.fillRect(x, y - scale * 2, linesize, scale * 4, TFT_GREEN);
  display.drawLine(x - scale * 1.3, y - scale * 1.3, x + scale * 1.3, y + scale * 1.3, TFT_GREEN);
  display.drawLine(x - scale * 1.3, y + scale * 1.3, x + scale * 1.3, y - scale * 1.3, TFT_GREEN);
  if (IconSize == LargeIcon) {
    display.drawLine(1 + x - scale * 1.3, y - scale * 1.3, 1 + x + scale * 1.3, y + scale * 1.3, TFT_GREEN);
    display.drawLine(2 + x - scale * 1.3, y - scale * 1.3, 2 + x + scale * 1.3, y + scale * 1.3, TFT_GREEN);
    display.drawLine(3 + x - scale * 1.3, y - scale * 1.3, 3 + x + scale * 1.3, y + scale * 1.3, TFT_GREEN);
    display.drawLine(1 + x - scale * 1.3, y + scale * 1.3, 1 + x + scale * 1.3, y - scale * 1.3, TFT_GREEN);
    display.drawLine(2 + x - scale * 1.3, y + scale * 1.3, 2 + x + scale * 1.3, y - scale * 1.3, TFT_GREEN);
    display.drawLine(3 + x - scale * 1.3, y + scale * 1.3, 3 + x + scale * 1.3, y - scale * 1.3, TFT_GREEN);
  }
  display.fillCircle(x, y, scale * 1.3, TFT_YELLOW);
  display.fillCircle(x, y, scale, TFT_GREEN);
  display.fillCircle(x, y, scale - linesize, TFT_YELLOW);
}
//#########################################################################################
void addfog(int x, int y, int scale, int linesize, bool IconSize) {
  if (IconSize == SmallIcon) {
    y -= 10;
    linesize = 1;
  }
  for (int i = 0; i < 6; i++) {
    display.fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, TFT_GREEN);
    display.fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, TFT_GREEN);
    display.fillRect(x - scale * 3, y + scale * 2.5, scale * 6, linesize, TFT_GREEN);
  }
}
//#########################################################################################
void Sunny(int x, int y, bool IconSize, String IconName) {
  int scale = Small;
  if (IconSize == LargeIcon) scale = Large;
  else y = y - 3; // Shift up small sun icon
  if (IconName.endsWith("n")) addmoon(x, y + 3, scale, IconSize);
  scale = scale * 1.6;
  addsun(x, y, scale, IconSize);
}
//#########################################################################################
void MostlySunny(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 3, offset = 5;
  if (IconSize == LargeIcon) {
    scale = Large;
    offset = 10;
  }
  if (scale == Small) linesize = 1;
  if (IconName.endsWith("n")) addmoon(x, y + offset, scale, IconSize);
  addcloud(x, y + offset, scale, linesize);
  addsun(x - scale * 1.8, y - scale * 1.8 + offset, scale, IconSize);
}
//#########################################################################################
void MostlyCloudy(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale, linesize);
}
//#########################################################################################
void Cloudy(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
    linesize = 1;
    addcloud(x, y, scale, linesize);
  }
  else {
    y += 10;
    if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
    addcloud(x + 30, y - 45, 5, linesize); // Cloud top right
    addcloud(x - 20, y - 30, 7, linesize); // Cloud top left
    addcloud(x, y, scale, linesize);       // Main cloud
  }
}
//#########################################################################################
void Rain(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}
//#########################################################################################
void ExpectRain(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}
//#########################################################################################
void ChanceRain(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}
//#########################################################################################
void Tstorms(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addtstorm(x, y, scale);
}
//#########################################################################################
void Snow(int x, int y, bool IconSize, String IconName) {
  int scale = Large, linesize = 3;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addsnow(x, y, scale, 5, IconSize);
}
//#########################################################################################
void Fog(int x, int y, bool IconSize, String IconName) {
  int linesize = 3, scale = Large;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addcloud(x, y - 5, scale, linesize);
  addfog(x, y - 5, scale, linesize, IconSize);
}
//#########################################################################################
void Haze(int x, int y, bool IconSize, String IconName) {
  int linesize = 3, scale = Large;
  if (IconSize == SmallIcon) {
    scale = Small;
    linesize = 1;
  }
  if (IconName.endsWith("n")) addmoon(x, y, scale, IconSize);
  addsun(x, y - 5, scale * 1.4, IconSize);
  addfog(x, y - 5, scale * 1.4, linesize, IconSize);
}
//#########################################################################################
void CloudCover(int x, int y, int CCover) {
  addcloud(x - 9, y - 3, Small * 0.5, 2); // Cloud top left
  addcloud(x + 3, y - 3, Small * 0.5, 2); // Cloud top right
  addcloud(x, y,         Small * 0.5, 2); // Main cloud
  display.setFont(&helvB08);
  drawString(x + 15, y - 5, String(CCover) + "%", LEFT);
}
//#########################################################################################
void Visibility(int x, int y, String Visi) {
  y = y - 3; //
  float start_angle = 0.52, end_angle = 2.61;
  int r = 10;
  for (float i = start_angle; i < end_angle; i = i + 0.05) {
    display.drawPixel(x + r * cos(i), y - r / 2 + r * sin(i), TFT_GREEN);
    display.drawPixel(x + r * cos(i), 1 + y - r / 2 + r * sin(i), TFT_GREEN);
  }
  start_angle = 3.61; end_angle = 5.78;
  for (float i = start_angle; i < end_angle; i = i + 0.05) {
    display.drawPixel(x + r * cos(i), y + r / 2 + r * sin(i), TFT_GREEN);
    display.drawPixel(x + r * cos(i), 1 + y + r / 2 + r * sin(i), TFT_GREEN);
  }
  display.fillCircle(x, y, r / 4, TFT_GREEN);
  display.setFont(&helvB08);
  drawString(x + 12, y - 3, Visi, LEFT);
}
//#########################################################################################
void addmoon(int x, int y, int scale, bool IconSize) {
  if (IconSize == LargeIcon) {
    display.fillCircle(x - 50, y - 55, scale, TFT_DARKGREY);
    display.fillCircle(x - 35, y - 55, scale * 1.6, TFT_LIGHTGREY);
  }
  else
  {
    display.fillCircle(x - 20, y - 12, scale, TFT_DARKGREY);
    display.fillCircle(x - 15, y - 12, scale * 1.6, TFT_LIGHTGREY);
  }
}
//#########################################################################################
void Nodata(int x, int y, bool IconSize, String IconName) {
  if (IconSize == LargeIcon) display.setFont(&helvB24); else display.setFont(&helvB10);
  drawString(x - 3, y - 8, "?", CENTER);
  display.setFont(&helvB08);
}

void DrawGraph(int x_pos, int y_pos, int gwidth, int gheight, float Y1Min, float Y1Max, String title, float DataArray[], int readings, boolean auto_scale, boolean barchart_mode) {
#define auto_scale_margin 0 // Sets the autoscale increment, so axis steps up in units of e.g. 3
#define y_minor_axis 5      // 5 y-axis division markers
  int maxYscale = -10000;
  int minYscale =  10000;
  int last_x, last_y;
  float x2, y2;
  if (auto_scale == true) {
    for (int i = 1; i < readings; i++ ) { // Adjusted graph range
      if (DataArray[i] >= maxYscale) maxYscale = DataArray[i];
      if (DataArray[i] <= minYscale) minYscale = DataArray[i];
    }
    maxYscale = round(maxYscale + auto_scale_margin); // Auto scale the graph and round to the nearest value defined, default was Y1Max
    Y1Max = round(maxYscale + 0.5);
    if (minYscale != 0) minYscale = round(minYscale - auto_scale_margin); // Auto scale the graph and round to the nearest value defined, default was Y1Min
    Y1Min = round(minYscale);
  }
  // Draw the graph
  last_x = x_pos;
  last_y = y_pos + (Y1Max - constrain(DataArray[1], Y1Min, Y1Max)) / (Y1Max - Y1Min) * gheight;
  display.drawRect(x_pos, y_pos, gwidth + 3, gheight + 2, TFT_GREEN);
  drawString(x_pos + gwidth / 2, y_pos - 17, title, CENTER);
  // Draw the data
  for (int gx = 0; gx < readings; gx++) {
    y2 = y_pos + (Y1Max - constrain(DataArray[gx], Y1Min, Y1Max)) / (Y1Max - Y1Min) * gheight + 1;
    if (barchart_mode) {
      x2 = x_pos + gx * (gwidth / readings) + 2;
      display.fillRect(x2, y2, (gwidth / readings) - 2, y_pos + gheight - y2 + 2, TFT_BLUE);
    } 
    else
    {
      x2 = x_pos + gx * gwidth / (readings - 1) + 1; // max_readings is the global variable that sets the maximum data that can be plotted
      display.drawLine(last_x, last_y, x2, y2, TFT_WHITE);
    }
    last_x = x2;
    last_y = y2;
  }
  //Draw the Y-axis scale
#define number_of_dashes 20
  for (int spacing = 0; spacing <= y_minor_axis; spacing++) {
    for (int j = 0; j < number_of_dashes; j++) { // Draw dashed graph grid lines
      if (spacing < y_minor_axis) display.drawFastHLine((x_pos + 3 + j * gwidth / number_of_dashes), y_pos + (gheight * spacing / y_minor_axis), gwidth / (2 * number_of_dashes), TFT_GREEN);
    }
    if (Y1Min < 1 && Y1Max < 10)
      drawString(x_pos - 3, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 1), RIGHT);
    else
      drawString(x_pos - 3, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 0), RIGHT);
  }
  for (int i = 0; i <= 2; i++) {
    drawString(15 + x_pos + gwidth / 3 * i, y_pos + gheight + 2, String(i), LEFT);
  }
  drawString(x_pos + gwidth / 2, y_pos + gheight +14, TXT_DAYS, CENTER);
}

//#########################################################################################
void drawString(int x, int y, String text, alignment align) {
  int16_t  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  display.setTextWrap(false);
  w = display.textWidth(text.c_str()); //Use display Width
  if (align == RIGHT)  x = x - w;
  if (align == CENTER) x = x - w / 2;
  display.setCursor(x, y + h);
  display.print(text);
}

void drawStringx(int x, int y, String text) {
  //display.setTextWrap(false);
  display.print(text);
}
//#########################################################################################
void drawStringMaxWidth(int x, int y, unsigned int text_width, String text, alignment align) {
  int16_t  x1, y1; //the bounds of x,y and w and h of the variable 'text' in pixels.
  uint16_t w, h;
  //display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  if (align == RIGHT)  x = x - w;
  if (align == CENTER) x = x - w / 2;
  display.setCursor(x, y);
  if (text.length() > text_width * 2) {
    display.setFont(&helvB10);
    text_width = 42;
    y = y - 3;
  }
  display.println(text.substring(0, text_width));
  if (text.length() > text_width) {
    display.setCursor(x, y + h + 15);
    String secondLine = text.substring(text_width);
    secondLine.trim(); // Remove any leading spaces
    display.println(secondLine);
  }
}
//#########################################################################################

//#########################################################################################
String Translate_EN_DE(String text) {
  if (text == "clear")            return "klar";
  if (text == "sunny")            return "sonnig";
  if (text == "mist")             return "Nebel";
  if (text == "fog")              return "Nebel";
  if (text == "rain")             return "Regen";
  if (text == "shower")           return "Regenschauer";
  if (text == "cloudy")           return "wolkig";
  if (text == "clouds")           return "Wolken";
  if (text == "drizzle")          return "Nieselregen";
  if (text == "snow")             return "Schnee";
  if (text == "thunderstorm")     return "Gewitter";
  if (text == "light")            return "leichter";
  if (text == "heavy")            return "schwer";
  if (text == "mostly cloudy")    return "größtenteils bewölkt";
  if (text == "overcast clouds")  return "überwiegend bewölkt";
  if (text == "scattered clouds") return "aufgelockerte Bewölkung";
  if (text == "few clouds")       return "ein paar Wolken";
  if (text == "clear sky")        return "klarer Himmel";
  if (text == "broken clouds")    return "aufgerissene Bewölkung";
  if (text == "light rain")       return "leichter Regen";
  return text;
  }

// This Code is to reset the Weatherdisplay every 30 Minutes to get new values in the forecast.
void EspReset() {
  // Schaltuhr für Wetterupdate
    timeClient.update();
    hours = (timeClient.getHours());
    minutes = (timeClient.getMinutes());
    seconds = (timeClient.getSeconds());
    
    //ESP Restart every 59 and 30 Minutes of an hour for OWM Update and reset the Display
     if((minutes == 59) && (seconds == 1)){
       ESP.deepSleep(1e6); 
        }

    else if((minutes == 30) && (seconds == 1)){
       ESP.deepSleep(1e6); 
         }
      
}

void pause10() {

unsigned long StartTime = millis();
while (true) {
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  if(ElapsedTime >= 10000){
    break;
  }
  }
}

