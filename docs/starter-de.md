# Tronferno Schnellstart-Anleitung


#### Aufbau der Hardware

Die [Hardware](hardware-de.md) besteht aus einem ESP32-Board und einem oder zwei 433.92 MHz Funkmodulen.

* [Benötigte Hardware](hardware-de.md)
* [Bauanleitung mit Olimex-ESP32-Gateway und CC1101 Board](esp32gw_cc1101-de.md)
* [Schaltbild Tronferno-MCU mit einem ESP32-Board und zwei Funkmodulen](schematic.pdf)

#### Installation der Firmware
* Nach dem Zusammenbau der Hardware muss die Tronferno-Firmware installiert (geflasht werden).
   * Die Tronferno-Firmware wird erstmalig via USB auf das ESP32-Board installiert.
      * [Tronferno-Firmware erstmalig flashen](starter_flash-de.md)
   * Neuere Versionen der Firmware (Updates) können nach Einrichtung des Netzwerks dann über die Weboberfläche online installiert werden (OTA-Update).
      * Navigation: ?->OTA

#### Netzwerkzugang einrichten

* Der Netzwerkzugang kann wahlweise über WLAN-Accesspoint per Webbrowser oder über Kommandozeile/FHEM/MenuTool eingerichtet werden
* WLAN-Accespoint: 
    * Beim Start versucht der Microcontroller eine Netzwerkverbindung aufzubauen. Schlägt dies fehl wird stattdessen ein [WLAN Accesspoint](network-de.md) gestartet.
    * Du kannst einen Laptop oder Smartphone mit diesem WLAN verbinden und dann die Weboberfläche über [http://192.168.4.1](http://192.168.4.1) erreichen.
        * Über diese Weboberfläche dann das gewünschte [Netzwerk einrichten](network-de.md) und dann neu starten.
        * Achtung: PCs funktionieren problemlos, aber auf manchem Android-10 oder neuer (Xiaomi)-Smartphones kann es Probleme geben den Webserver des ESP32 zu erreichen:
           * Mobile Datenverbindung des Smartphones deaktivieren
           * In den WLAN-Einstellungen von "tronferno" von DHCP auf statische IP-Adresse (192.168.4.x) umstellen
               * Chrome funktioniert nun. Firefox nicht. 
* USB:
   * [Kommandozeilenoptionen benutzen](mcu_config_cli-de.md)
   * [Netzwerk einrichten](network-de.md)

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

* HomeServer werden idR über [MQTT](mqtt.md) angebunden
* Für FHEM existiert neben MQTT spezielle Module die  etwas mehr Funktionalität bieten.
   * [FHEM-Homeserver Modul (USB oder TCP/IP)](https://github.com/zwiebert/tronferno-fhem)
* Automatisch erzeugte Konfigurationen für HomeServer findest du in der Weboberfläche.
   * Navigation: ?->[HOME]
      * HomeAssistant
      * FHEM
      * OpenHAB



#### Mehr Information

* [Oft gestellte Fragen (FAQ)](starter_faq-de.md)


    
     
#### Mehr Info
  * [GPIO pin numbers](pins.md)
  * [hardware](hardware.md)
  * [configuration](mcu_config.md)
  * [CLI](cli.md)
  * [network](network.md)
  * [HTTP](http.md)
  * [webserver](webserver.md)
  

