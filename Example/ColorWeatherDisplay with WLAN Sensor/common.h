#ifndef COMMON_H_
#define COMMON_H_

#include <Arduino.h>
#include <ArduinoJson.h>  //Version 7.xxx
#include <HTTPClient.h>

#include "forecast_record.h"
#include "common_functions.h"

//#########################################################################################
void Convert_Readings_to_Imperial() {
  WxConditions[0].Pressure = hPa_to_inHg(WxConditions[0].Pressure);
  WxForecast[1].Rainfall   = mm_to_inches(WxForecast[1].Rainfall);
  WxForecast[1].Snowfall   = mm_to_inches(WxForecast[1].Snowfall);
}

//#########################################################################################
// Problems with stucturing JSON decodes, see here: https://arduinojson.org/assistant/
bool DecodeWeather(WiFiClient& json, String Type) {
 
  // allocate the JsonDocument
  JsonDocument doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);
  // Test if parsing succeeds.
  if (error) {
   
    return false;
  }
  // convert it to a JsonObject
  JsonObject root = doc.as<JsonObject>();
  
  if (Type == "weather") {
    // All Serial.println statements are for diagnostic purposes and not required, remove if not needed
    WxConditions[0].lon         = root["coord"]["lon"].as<float>();                   
    WxConditions[0].lat         = root["coord"]["lat"].as<float>();                    
    WxConditions[0].Main0       = root["weather"][0]["main"].as<const char*>();        
    WxConditions[0].Forecast0   = root["weather"][0]["description"].as<const char*>();
    WxConditions[0].Forecast1   = root["weather"][1]["description"].as<const char*>(); 
    WxConditions[0].Forecast2   = root["weather"][2]["description"].as<const char*>(); 
    WxConditions[0].Icon        = root["weather"][0]["icon"].as<const char*>();       
    WxConditions[0].Temperature = root["main"]["temp"].as<float>();                    
    WxConditions[0].Pressure    = root["main"]["pressure"].as<float>();               
    WxConditions[0].Humidity    = root["main"]["humidity"].as<float>();                
    WxConditions[0].Low         = root["main"]["temp_min"].as<float>();                
    WxConditions[0].High        = root["main"]["temp_max"].as<float>();                
    WxConditions[0].Windspeed   = root["wind"]["speed"].as<float>();                  
    WxConditions[0].Winddir     = root["wind"]["deg"].as<float>();                     
    WxConditions[0].Cloudcover  = root["clouds"]["all"].as<int>();                     
    WxConditions[0].Visibility  = root["visibility"].as<int>();                        
    WxConditions[0].Rainfall    = root["rain"]["1h"].as<float>();                      
    WxConditions[0].Snowfall    = root["snow"]["1h"].as<float>();                      
    WxConditions[0].Country     = root["sys"]["country"].as<const char*>();            
    WxConditions[0].Sunrise     = root["sys"]["sunrise"].as<int>();                    
    WxConditions[0].Sunset      = root["sys"]["sunset"].as<int>();                     
    WxConditions[0].Timezone    = root["timezone"].as<int>();                         
  }
  
   if (Type == "forecast") {
    //Serial.println(json);
    Serial.print(F("\nReceiving Forecast period - "));
    JsonArray list                    = root["list"];
    for (byte r = 0; r < max_readings; r++) {
      
      WxForecast[r].Dt                = list[r]["dt"].as<int>();                                
      WxForecast[r].Temperature       = list[r]["main"]["temp"].as<float>();                    
      WxForecast[r].Low               = list[r]["main"]["temp_min"].as<float>();                
      WxForecast[r].High              = list[r]["main"]["temp_max"].as<float>();                
      WxForecast[r].Pressure          = list[r]["main"]["pressure"].as<float>();                
      WxForecast[r].Humidity          = list[r]["main"]["humidity"].as<float>();               
      WxForecast[r].Forecast0         = list[r]["weather"][0]["main"].as<const char*>();       
      WxForecast[r].Forecast1         = list[r]["weather"][1]["main"].as<const char*>();        
      WxForecast[r].Forecast2         = list[r]["weather"][2]["main"].as<const char*>();        
      WxForecast[r].Icon              = list[r]["weather"][0]["icon"].as<const char*>();        
      WxForecast[r].Description       = list[r]["weather"][0]["description"].as<const char*>(); 
      WxForecast[r].Cloudcover        = list[r]["clouds"]["all"].as<int>();                    
      WxForecast[r].Windspeed         = list[r]["wind"]["speed"].as<float>();                   
      WxForecast[r].Winddir           = list[r]["wind"]["deg"].as<float>();                    
      WxForecast[r].Rainfall          = list[r]["rain"]["3h"].as<float>();                      
      WxForecast[r].Snowfall          = list[r]["snow"]["3h"].as<float>();                      
      WxForecast[r].Pop               = list[r]["pop"].as<float>();                             
      WxForecast[r].Period            = list[r]["dt_txt"].as<const char*>();                    
    }
    //------------------------------------------
    float pressure_trend = WxForecast[2].Pressure - WxForecast[0].Pressure; // Measure pressure slope between ~now and later
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0; // Remove any small variations less than 0.1
    WxConditions[0].Trend = "0";
    if (pressure_trend > 0)  WxConditions[0].Trend = "+";
    if (pressure_trend < 0)  WxConditions[0].Trend = "-";
    if (pressure_trend == 0) WxConditions[0].Trend = "0";

    if (Units == "I") Convert_Readings_to_Imperial();
  }
  return true;
}
//#########################################################################################
String ConvertUnixTime(int unix_time) {
  // Returns either '21:12  ' or ' 09:12pm' depending on Units mode
  time_t tm = unix_time;
  struct tm *now_tm = gmtime(&tm);
  char output[40];
  if (Units == "M") {
    strftime(output, sizeof(output), "%H:%M %d/%m/%y", now_tm);
  }
  else {
    strftime(output, sizeof(output), "%I:%M%P %m/%d/%y", now_tm);
  }
  return output;
}
//#########################################################################################
//WiFiClient client; // wifi client object

bool obtain_wx_data(WiFiClient& client, const String& RequestType) {
  const String units = (Units == "M" ? "metric" : "imperial");
  client.stop(); // close connection before sending a new request
  HTTPClient http;    
  // Update for API 3.0 June '24
  //String uri = "/data/3.0/onecall?lat=" + LAT + "&lon=" + LON + "&appid=" + apikey + "&mode=json&units=" + units + "&lang=" + Language;
  String uri = "/data/2.5/" + RequestType + "?lat=" + LAT + "&lon=" + LON + "&appid=" + apikey + "&mode=json&units=" + units + "&lang=" + Language;
  if(RequestType != "weather")
  {
    uri += "&cnt=" + String(max_readings);
  }
  http.begin(client, server, 80, uri);
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    if (!DecodeWeather(http.getStream(), RequestType)) return false;
    client.stop();
    http.end();
    return true;
  }
  else
  {
    
    client.stop();
    http.end();
    return false;
  }
  http.end();
  return true;
}
#endif /* ifndef COMMON_H_ */
