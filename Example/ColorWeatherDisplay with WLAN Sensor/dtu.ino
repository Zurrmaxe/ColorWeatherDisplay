void aDtuData() {
    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(aDTU);
    int httpCode = http.GET();            // Send HTTP GET request
    if (httpCode > 0) {                   // Check if the request was successful
      DynamicJsonDocument doc(2048);      // Create JSON document (adjust the size accordingly)
      String payload = http.getString();  // Retrieve response from the website
      DeserializationError error = deserializeJson(doc, payload);  // Parse JSON data
        if (Debug >1) {serializeJsonPretty(doc, Serial); } ; // nur Debug
        atpwr =doc["inverters"][0]["AC"]["0"]["Power"]["v"];
        adpwr =doc["total"]["YieldDay"]["v"];
             
         } else Serial.println("Error retrieving METAR data");
    http.end();
   }
   return;
}
void bDtuData() {
    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(bDTU);
    int httpCode = http.GET();            // Send HTTP GET request
    if (httpCode > 0) {                   // Check if the request was successful
      DynamicJsonDocument doc(2048);      // Create JSON document (adjust the size accordingly)
      String payload = http.getString();  // Retrieve response from the website
      DeserializationError error = deserializeJson(doc, payload);  // Parse JSON data
        if (Debug >1) {serializeJsonPretty(doc, Serial); } ; // nur Debug
        btpwr =doc["inverters"][0]["AC"]["0"]["Power"]["v"];
        bdpwr =doc["total"]["YieldDay"]["v"];
       
         } else Serial.println("Error retrieving bdtu");
    http.end();
   }
   return;
}

void shellyData() {
    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(shelly);
    int httpCode = http.GET();            // Send HTTP GET request
    if (httpCode > 0) {                   // Check if the request was successful
      DynamicJsonDocument doc(2048);      // Create JSON document (adjust the size accordingly)
      String payload = http.getString();  // Retrieve response from the website
      DeserializationError error = deserializeJson(doc, payload);  // Parse JSON data
        if (Debug >1) {serializeJsonPretty(doc, Serial); } ; // nur Debug
        phasea = doc["emeters"][0]["power"]; 
        phaseb = doc["emeters"][1]["power"]; 
        phasec = doc["emeters"][2]["power"]; 
        netz = phasea+phaseb+phasec;
       } else Serial.println("Error retrieving METAR data");
    http.end();
   }
   return;
}

void DHTData() {
    if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(sensora);
    int httpCode = http.GET();            // Send HTTP GET request
    if (httpCode > 0) {                   // Check if the request was successful
      DynamicJsonDocument doc(2048);      // Create JSON document (adjust the size accordingly)
      String payload = http.getString();  // Retrieve response from the website
      DeserializationError error = deserializeJson(doc, payload);  // Parse JSON data
        if (Debug >1) {serializeJsonPretty(doc, Serial); } ; // nur Debug
        dhttemp = doc[0]["value"];
        dhthum = doc[1]["value"];
        dhtpress = doc[2]["value"]; //not in use, for future
       } else Serial.println("Error retrieving DGT11");
    http.end();
   }
   return;
}

void solar() {
  aDtuData();
  bDtuData();
  shellyData();
  DHTData();
  last = atpwr + btpwr + netz;
  solarleist = atpwr +btpwr;
  einspeise = solarleist + netz;
  energie();
 
}

void energie(){
  sprite.fillSprite(0);  // init empty sprit
  sprite.drawRect(2, 0, 369, 202,TFT_YELLOW);
  sprite.setFont(&helvB08); 
  sprite.setTextColor(TFT_WHITE);
  sprite.setCursor(30,2);
  sprite.print("Carport");
  sprite.setCursor(310,2);
  sprite.print("Garage");
  if (areach = 1) { 
  sprite.setFont(&helvB10); 
  sprite.setCursor(20,20);
  sprite.print(atpwr);
  sprite.drawString("W", 65, 20);
  sprite.setFont(&helvB10); 
  sprite.drawXBitmap(10,15,Sol5085,85,50,TFT_YELLOW);
  sprite.setTextColor(TFT_WHITE);
  sprite.setCursor(20, 45);
  sprite.setTextColor(TFT_WHITE);
  sprite.print(adpwr);
  sprite.drawString("W/h", 60, 45);
  }
 else {
  
 }
  
  if (breach = 1) { 
  sprite.setCursor(290,20);
  sprite.print(btpwr);
  sprite.drawString("W", 340, 20);
  sprite.setFont(&helvB10); 
  sprite.drawXBitmap(280,15,Sol5085,85,50,TFT_YELLOW);
  sprite.setTextColor(TFT_WHITE);
  sprite.setCursor(290, 45);
  sprite.setTextColor(TFT_WHITE);
  sprite.print(bdpwr);
  sprite.drawString("W/h", 330, 45);
}
 else {
 }
  sprite.setFont(&helvB18); 
  if (solarleist > 0) {
  sprite.drawXBitmap(150,5,Sun70,70,70,TFT_YELLOW);
  sprite.drawXBitmap(215,60,PfeilRechts,80,80,TFT_YELLOW);
  sprite.setCursor(160, 75);
  sprite.setTextColor(TFT_WHITE);
  sprite.print(solarleist);
   }
      else if (solarleist < 0) {
   }
    if (netz > 0) {
       sprite.setCursor(280, 100);
       sprite.drawXBitmap(270,143,Home8050,80,50,TFT_RED);
       sprite.drawXBitmap(20,130,Strom70,70,70,TFT_RED);
       sprite.drawXBitmap(110,150,PfeilGerade,150,30,TFT_RED);
       sprite.setTextColor(TFT_RED);
       sprite.setCursor(150,130);
       sprite.print(netz);   
       }
      else if (netz < 0) {
       sprite.setCursor(300, 90);
       sprite.setTextColor(TFT_YELLOW);
       sprite.print(solarleist + netz);
       sprite.drawXBitmap(20,130,Strom70,70,70,TFT_GREEN);
       sprite.drawXBitmap(270,143,Home8050,80,50,TFT_GREEN);
       sprite.drawXBitmap(80,60,PfeilLinks,80,80,TFT_GREEN);
       sprite.drawXBitmap(145,110,emoyes80,80,80,TFT_GREEN);
       sprite.setTextColor(TFT_GREEN);
       sprite.setCursor(20, 90);
       sprite.print(netz);
        }
        //inside DHT11
        sprite.setFont(&helvB12);
        sprite.setTextColor(TFT_WHITE);
        sprite.drawRect(2, 204, 184, 53, TFT_YELLOW); // temp outline
        float humi  = dht_sensor.readHumidity();
        float tempC = dht_sensor.readTemperature();
        sprite.setCursor(5,225);
        sprite.drawXBitmap(7,210,HausT,40,40,TFT_YELLOW);
        sprite.setCursor(60,210);
        sprite.print(tempC);
        sprite.print(" 'C: ");
        sprite.setCursor(60,230);
        sprite.print(humi);
        sprite .println(" % RH");
        //outside DHT11 
        sprite.setFont(&helvB12);
        sprite.drawXBitmap(197,217,Outside,40,40,TFT_YELLOW);
        sprite.setCursor(250,206);
        sprite.print(dhttemp);
        Serial.println(dhttemp);
        sprite.print(" 'C: ");
        sprite.setCursor(250,220);
        sprite.print(dhthum);
        Serial.println(dhthum);
        sprite .println(" % RH");
        sprite.setCursor(250,234);
        sprite.print(dhtpress);
        Serial.println(dhtpress);
        sprite .println(" hPA");
        sprite.drawRect(187, 204, 184, 53, TFT_YELLOW); // temp outline
        sprite.pushSprite(425,0); 
}

