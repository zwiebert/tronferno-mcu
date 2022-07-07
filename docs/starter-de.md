# Tronferno Schnellstart-Anleitung


#### Aufbau der Hardware

Die [Hardware](hardware-de.md) kann leicht selber zusammen gebaut werden, und besteht aus einem ESP32-Board und einem oder zwei Funkmodulen für die Funkfrequenz von 433.92 MHz.

* [Benötigte Hardware](hardware-de.md)
* [Bauanleitung mit Olimex-ESP32-Gateway und Neuftech-CC1101](esp32gw_cc1101-de.md)
* [Schaltbild Tronferno-MCU mit einem ESP32-Board und zwei Funkmodulen](schematic.pdf)

#### Installation der Firmware
Die Tronferno-Firmware wird erstmalig via USB auf das ESP32-Board installiert. Spätere Updates können bequem online durchgeführt werden (Updates liegen auf dem GitHUB-Server).
* [Tronferno-Firmware erstmalig flashen](starter_flash-de.md)

#### Netzwerkzugang und Webinterface konfigurieren

Die erstmalige Konfiguration kann über einen WLAN-Accesspoint durchgeführt werden (oder auch mit dem menutool Programm, direkt nach dem Flashen).  Dabei sollten die WLAN Zugangsdaten eingerichtet werden. Wenn das Board LAN unterstützt, kann dieses anstelle von WLAN benutzt werden.  Die weitere Konfiguration sollte dann nach und nach über diesen Webserver durchgeführt werden.  Außerdem sollte der integrierte Webserver aktiviert werden und bei Bedarf Benutzername und Passwort um den Zugriff auf die Weboberfläche einzuschränken.

#### Weboberfläche öffnen

 Zum Aufruf der Weboberfläche wird die vom Router vergebene IP-Adresse benötigt. In der Fritz-Box-Oberfläche findet sich  diese unter Heimnetzwerk und dem Gerätenamen "espressif".  Wenn über USB mit einem FHEM-Server verbunden, taucht die IP Adresse außerdem als Reading im Modul TronfernoMCU auf.
 
Das Webinterface erlaubt Konfiguration des Tronferno-Controllers und die Steuerung und Programmierung der Rollladenmotore.

* [webserver](webserver.md)

#### Weitere Konfiguration über die Weboberfläche

* Festlegen der GPIO-Pins an welchen die Funksender/Empfänger angeschlossen wurden.
* ID der originalen 2411 Zentrale eintragen (Zettel im Batteriefach hinter den Batterien).  Diese ID kann auch automatisch konfiguriert werden (Knopf unter dem Text-Eingabefeld drücken und dann STOP auf der 2411 Zentrale drücken).
* Einstellen der Anzahl Rolläden pro Gruppe, so dass unbenutzte Gruppen und Empfänger nicht die Weboberfläche zumüllen und unnütze Datenabfragen zu nicht existierenden Rolläden vermieden werden können.


#### Homeserver-Integration

Für den FHEM-Homeserver existiert ein spezilelles Tronferno-Modul. Andere Homeserver (aber auch FHEM, falls man das Modul nicht benutzen möchte) können via MQTT mit Tronferno kommunizieren.

* [FHEM-Homeserver Modul (USB oder TCP/IP)](https://github.com/zwiebert/tronferno-fhem)


#### Mehr Information

* [Oft gestellte Fragen (FAQ)](starter_faq-de.md)



------------------
  
##### Homeserver die MQTT unterstützen (OpenHAB, FHEM, ...)
* MQTT muss dazu (z.B. mit dem menutool oder der Weboberfläche) aktiviert und konfiguriert werden
* [weitere Infos](https://github.com/zwiebert/tronferno-mcu-bin/blob/master/README.md)
     
##### Android-App Tronferno
* GitHub: [tronferno-andro](https://github.com/zwiebert/tronferno-andro)
           
##### Terminal-Programm (USB oder TCP/IP Port 7777) oder Telnet (Port 7777)
* Im Terminalprogramm werden Kommandozeilen nur mit einem Semikolon beendet
* In Telnet werden Kommandozeilen mit Semikolon und Eingabe-Taste (Enter/Return-Taste) beendet
     
     
#### Mehr Info
  * [GPIO pin numbers](pins.md)
  * [hardware](hardware.md)
  * [configuration](mcu_config.md)
  * [CLI](cli.md)
  * [network](network.md)
  * [HTTP](http.md)
  * [webserver](webserver.md)
  

