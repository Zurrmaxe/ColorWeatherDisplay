![Colorstation](https://github.com/Zurrmaxe/ColorWeatherDisplay/blob/6b400cef94f60ac23cd17803fd36b069af87d63d/Weatherstation%20Kopie.jpg)

Hinweis : Das Programm befindet sich noch in der Testphase und wird ständig erweitert. Es ist keine fertige Version sondern nur eine Versuch, verschiedene Anwendungen auf einem Display zusammenzufassen. Es sind noch einige Fehler vorhanden.

Dieses war mein erster Versuch, eine Wetterstation mit der Anzeige einer Solaranlage zu kombinieren. 
Hierzu musste ich den den Code der Wetterstation auf das neue Farbdisplay umgeschreiben und die Aufteilung der einzelenen Felder neu geregen.
Ebenso mussten die Internetanfragen für die Solaranlagen und den WLAN-Sensoren neu geschrieben und angepaßt werden.
Das Problem war und ist, dass die WLAN-Sensoren und die Solaranlagensteurung alle paar Sekunden die Daten holen sollen, die Wetterstation aber nur alle 30 Minuten.
An einer sauberen Lösung wird noch gearbeitet.

![Colorstation](https://github.com/user-attachments/assets/abc309b8-a76e-4789-b62d-9e85a6fdbd5d)


Achtung ! Dieses sind Test-Versionen und noch nicht fertig. 
Diese Wetterstation wird laufend erweitert und verändert. Es sind daher noch Fehler im Code und müssen nach und nach behoben werden ! 

Wenn etwas nicht funktitoniert, bitte selber anpassen und ändern. 

Grundlage für ein funktionierendes Display sind folgende Internetseiten:

https://github.com/lovyan03/LovyanGFX/discussions/424.

https://forum.squareline.io/t/howto-esp32-8048s070-board-for-arduino/1098.

https://www.haraldkreuzer.net/aktuelles/erste-schritte-mit-dem-sunton-esp32-s3-7-zoll-display-lovyangfx-und-lvgl.

Diese Internetseiten sind Grundlage und Arbeitsmaterial für das Display. Bitte vorher sorgfälltig durchlesen. 

Plichtlektüre !!

Als IDE wird Arduino 2.32 benutzte. Der Boardmanger für das ESP32 von Espressif muss unbedingt auf 2.0.17 eingestellt werden.
Weitere Einstellungen der Werkzeuge sind unter "Settings" zu finden.

Zum eigentlichen Code :

Dieser Sketch ist eine Modifikation der bekannten Wetterstation von G6EJD. 

Daher sollte man sich die entsprechende Seite einmal durchlesen um alle Funktionen zu begreifen und zu verstehen.

Hier der Link zur originalen Wetterstation :

https://github.com/G6EJD/ESP32-e-Paper-Weather-Display. 

All Credits goes to G6EJD. This software, the ideas and concepts is Copyright (c) David Bird 2014 and beyond.  All rights to this software are reserved. More information about the weather-station on his Github-Site.

Diese Wetterstation wurde von mir an das 7-Zoll Farbdisplay ESP32-8048S070 angepasst.

https://www.makerfabs.com/sunton-esp32-s3-7-inch-tn-display-with-touch.html

Die Wetteranzeige wird/soll immer alle 30 Minuten aktualisert

In der ersten Version mit Solaranzeige habe ich den "gelben Bereich" erschaffen. 

In diesem Bereich könne auch eigene Anwendungen oder Anzeigen eingebaut werden. 

Diese laufen hier seperat als „Sprite“ (siehe TFTeSPI = > sprite) und werden regelmäßig durch "void loop()" erneuert. 

Daher können hier und nur hier auch aktuelle Daten angezeigt werden, welche nicht nur alle 30 Minuten abgefragt werden sollen.

In dem "gelben" Bereich wurde von mir zunächst die Anzeigen meiner Solaranlage eingebaut.

Diese Solaranlage besteht aus zwei Hoymiles HMT2250T Invertern und ein Shelly 3EM zur Verbrauchsmessung im Haus.

Die Hoymiles-Inverter werden zusammen mit zwei OpenDTU-ESP32 (Nulleinspeisung) betrieben. 
Infos: https://github.com/tbnobody/OpenDTU  und https://github.com/helgeerbe/OpenDTU-OnBattery

Es wird die IP der OpenDTU-ESP32 und die jeweilige Inverter-Nummer benötigt. 

Mein Code im Script funktioniert ausschließlich NURr mit den OpenDTU-Battery den Hoymiles HMT2250T und dem Shelly 3EM ! 

Sollten andere Geräte bzw. Modelle verwendet werden, ist der Code entsprechend anzupassen ! Hierfür kann ich keinen Service und Hilfe anbieten ! 

Die erforderlichen Daten der OpenDTUs werden als JSON-File ausgelesen und angezeigt. Im Internet gibt es genug Anleitungen dazu, wie dieses im Einzelnen umzusetzen ist und ensprechend angepasst werden kann.

Weiterhin befindet sich im unteren Bereich des gelben Bereiches eine Anzeige für die Innen- und Aussentemperatur. Es werden DHT11,DHT22 oder BME280 pp. als Sensoen verwendet.

Ein Sensor ist direkt am Display angeschlossen, der anderer wird über einen ESP32 und dem WLAN  mit dem Display verbunden.

Die Sensoren bestehen aus einem ESP32-WROOM mit einem DHT11, DHT22 oder BME280 Sensor. Die Daten werden über das WLAN-Funknetz zum Display übertragen und dort ausgewertet.
Man kann auch andere Sensoren an dem ESP32-WROOM anschließen und die Werte überragen lassen. 
einfacher Aufbau auf Lochrasterkarten reicht aus. Es können alle Sensoren verwendet werden.  Applikationen dazu gibt es genügent im Netz.
So wäre es möglich, neben Wind und Luftfdruck auch Regendaten zu übertragen. Auch die Lichtstärke kann gemessen werden. 
Dazu habe ich einmal einen einfachen LDR zum testen eingebaut.

Ebenso kann auch man mehrere Sensoren benutzen. Einen fürs Wohnzimmer, einen für den Keller , die Garage oder das Gartenhaus.


![Colorstation new](https://github.com/Zurrmaxe/ColorWeatherDtu/blob/7c9367bba9eeea2a3b3b8ae7899596b93c5ad604/WLAN1.jpg)

Hier der 2.WLAN-Sensor für eine zweite Temperaturanzeige (Garage / Keller pp.)

![Colorstation new](https://github.com/Zurrmaxe/ColorWeatherDtu/blob/25f187e03a9b11aa9a980155490c1e9ca6ed305a/WLAN2.jpg)



Den Ideen sind keine Grenzen gesetzt.

Damit ist mein Display universell für alles im Haushalt einsetzbar. 
Mit den WLAN-Sensoren könnten auch Schaltbefehle ausgelöst werden.  
Dazu must der Code entsprechend erweitert werden. (Http-Post). 

Hierzu bitte folgende Hinweise lesen:

https://microcontrollerslab.com/esp32-rest-api-web-server-get-post-postman/


Neues Update vom Oktober 2024:

Hier eine neue Display, diesmal ohne (!) die Anzeige der Solaranlage

Dafür mit einer Anzeige der Wolkendecke (neu) und eine größeren Niederschlagsanzeige.

Angeschlossen könne auch hier wieder diverse Sensoren auf WLAN-Basis. 

![Colorstation new](https://github.com/Zurrmaxe/ColorWeatherDtu/blob/41fbac06e993acbfadbc69fdc2b3b2a37730f305/neue%20version.jpg)

Neues Update vom 23.11.24:

Die Aufteilung im Display wurde etwas geändert.
Farben wurden angepasst. Der Regen wird mit blauen Balken angezeigt, Schnee mit weißen Balken.
Ich hoffe das es funktioniert.
Die Aktualisierung der Wetterdatten wurde überarbeitet und arbeit jetzt zuverlässiger.

Es ist noch keine Nachtabschaltung der Displaybeleuchtung eingebaut. Diese folgt vermutlich mit der nächsten Version.

![Colorstation new](https://github.com/Zurrmaxe/ColorWeatherDtu/blob/1b22db09ef8fcf1b90c9ef2de0ea99b98d7ae641/231124.jpg)

Hier mal eine ältere Version mit Schneefall :

![Colorstation new](https://github.com/Zurrmaxe/ColorWeatherDtu/blob/427cf98698a520ca46a23a2bf7bd6773d3827ae4/231124A.jpg)

Version 24.11.2024 mit kleinern Verbesserungen und Änderungen an der Grafik

![Colorstation new](https://github.com/Zurrmaxe/ColorWeatherDtu/blob/ca3d1c1dfed837a4f9e77ad2635e5faaa2ade7b9/241124.jpg)


Achtung ! Wichtiger Hinweis:

Der Sketch darf ausschließlich für private, nicht kommerzielle Zwecke verwendet werden. 
Jegliche gewerbliche Verwendung ist untersagt. Die weiteren Rechte der anderen Autoren bleiben hiervon unberührt. 
Bei jeder Vervielfältigung sind die rechte der jeweiligen Lizenzinhaber zu beachten. 


Thanks to Bodmer, author of the TFT_eSPI library, for the inspiration to create this library.
Thanks to all authors of the OpenDTU-Projekt and all other Users of github for insprations.
Thanks to Adafruit Industries for publishing AdafruitGFX, which is the basis for TFT_eSPI. 
クレジット Credits
Inspiration: Bodmer 
Author: lovyan03 
Contributors: 
ciniml 
mongonta0716 
tobozo 
G6EJD
