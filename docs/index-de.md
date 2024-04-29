|Change Language: Deutsch, [English](index.md)|

# Programmierzentrale, Funksignalverstärker und Netzwerk-Hub für Fernotron

Die tronferno-mcu Firmware implementiert die Funktionen der originale Fernotron Geräte 2411-Programmierzentrale und 2427-Funksignalverstärker.

Zusätzlich werden moderne Funktionen wie Web-Bedienoberfläche und Heim-Server Integration geboten.

Der Tronferno vermittelt zwischen dem Netzwerk (WebApp, MQTT, Homeserver) und der Fernotron Installation. Er sendet (und empfängt) Funk-Kommandos, speichert Informationen zu den Fernotron Geräten, und anderes mehr.

### Dokumentation

Möchtest du mehr erfahren, sind hier Links zu den Themen:
  * [Funktionsumfang](features.md)
  * [Aufbau und Inbetriebnahme](starter-de.md)
  * [Fragen+Antworten, Problemlösungen](starter_faq-de.md)


## Repositories


 * [tronferno-mcu](https://github.com/zwiebert/tronferno-mcu) Source Distribution für Software Entwickler

 * [tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin): Binär-Distribution für Benutzer


 * [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem): Optionales Modul zur Integration in den Heim-Server FHEM. Stattdessen sollte, wenn ausreichend, nun MQTT benutzt werden.



### History
 * 2022: WebGUI finished. All main features implemented.
 * 2021: Port WebGUI to Svelte/Tailwind
 * 2020: shutter positions implemented
 * 2018: Moving to ESP32/esp-idf
 * 2017: Moving to ESP8266/NonOS
 * 2017: Moved to ATmega328p. Support for timer programming.
 * 2011: First version on ATmega168/AVRStudio. Limited to plain commands up/down/stop/...

### Kontakt

* Project-Autor und Wartung: Bert Winkelmann <tf.zwiebert@online.de>
* Zum Melden von Bugs kannst du auch ein Issue auf GitHub eröffnen. English oder Deutsch.