Hiinweis : Das Programm befindet sich noch bei mir in der Testphase und wird hier vermutlich am Wochenende hochgeladen. 

![Colorstation](https://github.com/user-attachments/assets/abc309b8-a76e-4789-b62d-9e85a6fdbd5d)


Achtung ! Dieses ist eine Beta-Version und noch nicht fertig. Sie wird laufend erweitert. Es sind noch  Fehler im Script und müssen noch behoben werden ! 
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

https://github.com/G6EJD/ESP32-e-Paper-Weather-Display. All Credits goes to G6EJD. This software, the ideas and concepts is Copyright (c) David Bird 2014 and beyond.  All rights to this software are reserved. More information about the weather-station on his Github-Site.

Diese Wetterstation wurde von mir an das 7-Zoll Farbdisplay ESP32-8048S070 angepasst.

https://www.makerfabs.com/sunton-esp32-s3-7-inch-tn-display-with-touch.html

Die Wetteranzeige wird immer alle 30 Minuten durch einen Reset erneuert.

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
Ein Sensor ist direkt am Display angeschlossen, der anderer DHT11 wird über einen ESP32 und ESP_NOW (WLAN) mit dem Display verbunden.
Infos u.a. https://www.electrosoftcloud.com/en/esp32-wifi-and-esp-now-simultaneously/

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
