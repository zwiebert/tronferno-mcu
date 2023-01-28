# Tronferno Schnellstart-Anleitung


#### Aufbau der Hardware

Die [Hardware](hardware-de.md) besteht aus nur einem ESP32-Board und einem oder zwei 433.92 MHz Funkmodulen.

* [Benötigte Hardware](hardware-de.md)
* [Bauanleitung mit Olimex-ESP32-Gateway und CC1101 Board](esp32gw_cc1101-de.md)
* [Schaltbild Tronferno-MCU mit einem ESP32-Board und zwei Funkmodulen](schematic.pdf)

#### Installation der Firmware
Die Tronferno-Firmware wird erstmalig via USB auf das ESP32-Board installiert. Spätere Updates können bequem online durchgeführt werden (Updates liegen auf dem GitHUB-Server).
* [Tronferno-Firmware erstmalig flashen](starter_flash-de.md)

#### Netzwerkzugang einrichten

Beim Start versucht der Microcontroller eine Netzwerkverbindung aufzubauen. Schlägt dies fehl wird [WLAN Accesspoint](network-de.md) eingerichtet.
Man kann sein Laptop oder Smartphone mit diesem WLAN verbinden und dann die Weboberfläche über [http://192.168.4.1](http://192.168.4.1) erreichen.
Dort dann das gewünschte Netzwerk einrichten.


#### Webserver aktivieren



#### Weboberfläche verwenden

* Der Webserver sollte  vom User bei der [Einrichtung des Netzwerkzugangs](network-de.md) aktiviert werden 
    * (CLI: http-enable=1)

* Zum Aufruf der Weboberfläche wird die vom Router vergebene IP-Adresse benötigt.
    * In der Fritz-Box-Oberfläche findet sich  diese unter Heimnetzwerk und dem Gerätenamen "tronferno" (vor 0.12.1.4: "espressif").
    * Wenn über USB mit einem FHEM-Server verbunden, taucht die IP Adresse außerdem als Reading im Modul TronfernoMCU (tfmcu) auf.
 
* Das Webinterface erlaubt Konfiguration des Tronferno-Controllers und die Steuerung und Programmierung der Fernotron Empfänger.
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
  

