|Change Language: Deutsch, [English](index.md)|

# Programmierzentrale, Funksignalverstärker und Netzwerk-Hub für Fernotron Installationen

Die tronferno-mcu Mikrocontroller-Firmware implementiert die Funktionen der originalen Fernotron 2411-Programmierzentrale und des 2427-Funksignalverstärker.

Zusätzlich werden moderne Funktionen wie Web-Bedienoberfläche und Heim-Server Integration geboten.

Der Tronferno vermittelt zwischen dem Netzwerk (WebApp, MQTT, Heimserver) und der Fernotron-Installation. Er sendet (und empfängt) Funkkommandos, speichert Informationen zu den Fernotron-Geräten, und anderes mehr.

### Dokumentation

  * [Funktionsumfang](features.md)
  * [Aufbau und Inbetriebnahme](starter-de.md)
  * [Fragen+Antworten, Problemlösungen](starter_faq-de.md)


## Repositories

 * [tronferno-mcu](https://github.com/zwiebert/tronferno-mcu) Source Distribution für Software Entwickler

 * [tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin): Binär-Distribution für Benutzer


 * [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem): Optionales Modul zur Integration in den Heim-Server FHEM. Stattdessen sollte, wenn ausreichend, nun MQTT benutzt werden.



### Projekt-Historie
 * 2011: Erstelle erste Version für einfach Kommandos. Läuft auf ATmega168/AVRStudio. 
 * 2017: Wechsel auf ATmega328p.
 * 2017: Implementiere Programmierung der internen Timer
 * 2017: Wechsel auf ESP8266/NonOS
 * 2018: Wechsel auf ESP32/esp-idf
 * 2020: Implementiere Rollladen Positionen
 * 2021: Baue Web-App mit Svelte/Tailwind statt Javascript/CSS
 * 2022: Komplettiere Web-App. Alle Funktionen sind jetzt darüber steuerbar.

### Kontakt

* [Fehler/Bug-Meldungen, Feature-Requests](https://github.com/zwiebert/tronferno-mcu/issues) (Englisch,Deutsch).
* Project-Autor und Wartung: Bert Winkelmann <tf.zwiebert@online.de> (Deutsch,Englisch)
