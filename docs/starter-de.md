# Tronferno Schnellstart-Anleitung


#### Aufbau der Hardware

Die Hardware kann leicht selber zusammen gebaut werden, und besteht aus einem ESP32-Board und einem CC1101 Transceiver (oder alternativ einem Sendemodul und einem (optionalen) Empfängermodul). Die Funkfrequenz ist 433.92 MHz. Beim Kauf eines CC1101 Moduls ist der Beschreibung zu entnehmen, ob dieses für Funkfrequenz im 43x MHz Bereich gedacht ist, und nicht etwa für den 86x MHz Bereich.

* [Schaltbild](schematic-1.png)

#### Installation der Firmware
Die Tronferno-Firmware wird erstmalig via USB auf das ESP32-Board installiert. Spätere Updates können bequem online durchgeführt werden (Updates liegen auf dem GitHUB-Server).  Das Flashen kann mit dem menutool Programm erfolgen.

* [Tronferno-Firmware flashen](starter_flash-de.md)

#### Netzwerkzugang und Webinterface konfigurieren

Die erstmalige Konfiguration kann über einen WLAN-Accesspoint durchgeführt werden (oder auch mit dem menutool Programm, direkt nach dem Flashen).  Dabei sollten die WLAN Zugangsdaten eingerichtet werden. Wenn das Board LAN unterstützt, kann dieses anstelle von WLAN benutzt werden.  Die weitere Konfiguration sollte dann nach und nach über diesen Webserver durchgeführt werden.  Außerdem sollte der integrierte Webserver aktiviert werden und bei Bedarf Benutzername und Passwort um den Zugriff auf die Weboberfläche einzuschränken.

#### Weboberfläche öffnen

 Zum Aufruf der Weboberfläche wird die vom Router vergebene IP-Adresse benötigt. In der Fritz-Box-Oberfläche findet sich  diese unter Heimnetzwerk und dem Gerätenamen "espressif".
 
Das Webinterface erlaubt Konfiguration des Tronferno-Controllers und die Steuerung und Programmierung der Rollladenmotore.

* [webserver](webserver.md)

#### Weitere Konfiguration über die Weboberfläche

* Festlegen der GPIO-Pins an welchen die Funksender/Empfänger angeschlossen wurden.
* ID der originalen 2411 Zentrale eintragen (Zettel im Batteriefach hinter den Batterien).  Diese ID kann auch automatisch konfiguriert werden (Knopf unter dem Text-Eingabefeld drücken und dann STOP auf der 2411 Zentrale drücken).
* Einstellen der Anzahl Rolläden pro Gruppe, so dass unbenutzte Gruppen und Empfänger nicht die Weboberfläche zumüllen und unnütze Datenabfragen zu nicht existierenden Rolläden vermieden werden können.


#### Homeserver-Integration

Für den FHEM-Homeserver existiert ein spezilelles Tronferno-Modul. Andere Homeserver (aber auch FHEM, falls man das Modul nicht benutzen möchte) können via MQTT mit Tronferno kommunizieren.

* [FHEM-Homeserver Modul (USB oder TCP/IP)](https://github.com/zwiebert/tronferno-fhem)


------------------

#### Funkmodule

Fernotron sendet und empfängt auf 434 MHz (433,92 MHz exakt).

Siehe: [hardware](hardware.md)
        
  
#### Benutzerschnittstellen
  
  
##### Web-Browser
* [network](network.md)
* [HTTP](http.md)
* [webserver](webserver.md)
        
     

     
##### Homeserver die MQTT unterstützen (OpenHAB, FHEM, ...)
* MQTT muss dazu (z.B. mit dem menutool) aktiviert und konfiguriert werden
* [weitere Infos](https://github.com/zwiebert/tronferno-mcu-bin/blob/master/README.md)
     
##### Android-App Tronferno
* GitHub: [tronferno-andro](https://github.com/zwiebert/tronferno-andro)
           
##### Terminal-Programm (USB oder TCP/IP Port 7777)
* Lokal-Echo sollte im Terminalprogramm aktiviert werden um die eigenen Eingaben sehen zu können
     
     
#### Mehr Info
  * [GPIO pin numbers](pins.md)
  * [hardware](hardware.md)
  * [configuration](mcu_config.md)
  * [CLI](cli.md)
  

