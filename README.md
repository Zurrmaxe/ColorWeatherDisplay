Hinweis : Das Programm befindet sich noch in der Testphase und wird ständig erweitert. Es ist keine fertige Version sondern nur eine Versuch, verschiedene Anwendungen auf einem Display zusammenzufassen. Es sind noch einige Fehler vorhanden.

![Colorstation](https://github.com/user-attachments/assets/abc309b8-a76e-4789-b62d-9e85a6fdbd5d)


Achtung ! Dieses sind Test-Versionen und noch nicht fertig. Sie werden laufend erweitert. Es sind noch Fehler im Script und müssen noch behoben werden ! 
Wenn etwas nicht funktitoniert, bitte selber anpassen und ändern. 

Grundlage für ein funktionierendes Display sind folgende Internetseiten:

https://github.com/lovyan03/LovyanGFX/discussions/424.

https://forum.squareline.io/t/howto-esp32-8048s070-board-for-arduino/1098.

https://www.haraldkreuzer.net/aktuelles/erste-schritte-mit-dem-sunton-esp32-s3-7-zoll-display-lovyangfx-und-lvgl.

Diese Internetseiten sind Grundlage und Arbeitsmaterial für das Display. Bitte vorher sorgfälltig durchlesen.

Als IDE wird Arduino 2.32 benutzte. Der Boardmanger für das ESP32 von Espressif muss unbedingt auf 2.0.17 eingestellt werden.
Weitere Einstellungen sind unter "Settings" zu finden.

Zum eigentlichen Code :

Dieser Sketch ist eine Modifikation der bekannten Wetterstation von G6EJD. 

Daher sollte man sich die entsprechende Seite einmal durchlesen um alle Funktionen zu begreifen.

https://github.com/G6EJD/ESP32-e-Paper-Weather-Display. 

All Credits goes to G6EJD. This software, the ideas and concepts is Copyright (c) David Bird 2014 and beyond.  All rights to this software are reserved. More information about the weather-station on his Github-Site.

Diese Wetterstation wurde von mir an das 7-Zoll Farbdisplay ESP32-8048S070 angepasst.

https://www.makerfabs.com/sunton-esp32-s3-7-inch-tn-display-with-touch.html

Die Wetteranzeige wird immer alle 30 Minuten aktualisert

Zusätzlich habe ich den gelben Bereich erschaffen. 

In diesem Bereich könne eigene Anwendungen oder Anzeigen eingebaut werden. 
Diese laufen hier als „Sprite“ und werden regelmäßig erneuert. 

Daher können hier und nur hier auch aktuelle Daten angezeigt werden.

In dem gelben Bereich wurde von mir die Anzeigen meiner Solaranlage eingebaut.

Diese Solaranlage besteht aus zwei Hoymiles HMT2250T Invertern und ein Shelly 3EM zur Verbrauchsmessung im Haus.

Die Hoymiles-Inverter werden zusammen mit zwei OpenDTU-ESP32 (Nulleinspeisung) betrieben. 
Infos: https://github.com/tbnobody/OpenDTU  und https://github.com/helgeerbe/OpenDTU-OnBattery

Es wird die IP der OpenDTU-ESP32 und die jeweilige Inverter-Nummer benötigt. 

Mein Code im Script funktioniert ausschließlich nur mit den OpenDTU-Battery den Hoymiles HMT2250T und dem Shelly 3EM ! 
Sollten andere Geräte verwendet werden, ist der Code entsprechend anzupassen ! Hierfür kann ich keinen Service und Hilfe anbieten ! 

Die erforderlichen Daten der OpenDTUs werden als JSON-File ausgelesen und angezeigt. Im Internet gibt es genug Anleitungen dazu, wie dieses im Einzelnen umzusetzen ist und ensprechend angepasst werden kann.

Weiterhin befindet sich im unteren Bereich des gelben Bereiches eine Anzeige für die Innen- und Aussentemperatur. Es werden DHT11 als Sensoen verwendet.
Ein Sensor ist direkt am Display angeschlossen, der anderer DHT11 wird über einen ESP32 und dem WLAN  mit dem Display verbunden.

Die Sensoren bestehen aus einem ESP32-WROOM mit einem DHT11, DHT22 oder BME280 Sensor. Die Daten werden über das WLAN-Funknetz zum Display übertragen und dort ausgewertet.
Man kann auch andere Sensoren an dem ESP32-WROOM anschließen und die Werte überragen lassen. 

So wäre es möglich, neben Wind und Luftfdruck auch Regendaten zu übertragen. Auch die Lichtstärke könnte gemessen werden. Dazu habe ich einmal einen einfachen LDR zu testen eingebaut.
Ebenso kann auch man mehrere Sensoren benutzen. 
Eine fürs Wohnzimmer, einen für den Keller , die Garage oder das Gartenhaus.

Den Ideen sind keine Grenzen gesetzt.

Damit ist mein Display universell für alles im Haushalt einsetzbar. Mit den WLAN-Sensoren könnten auch Schaltbefehle ausgelöst werden. 
Dazu must der Code entsprechend erweitert werden. (Http-Post). 

Hierzu bitte folgende Hinweise lesen:

https://microcontrollerslab.com/esp32-rest-api-web-server-get-post-postman/


Neues Update vom Oktober 2024:

Hier mein neues Display, diesmal ohne (!) die Anzeige der Solaranlage

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
