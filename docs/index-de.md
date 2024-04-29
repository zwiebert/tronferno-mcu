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
 * 2022: Web-App komplettiert. Alle Funktionen sind jetzt darüber steuerbar.
 * 2021: Web-App nun erzeugt mit Svelte/Tailwind statt Javascript/CSS
 * 2020: Rollladen Positionen anzeigen und anfahren nun möglich
 * 2018: Umstieg auf ESP32/esp-idf
 * 2017: Umstieg auf ESP8266/NonOS
 * 2017: Umstieg auf ATmega328p. Interne-Timer programmieren nun möglich
 * 2011: Erste Version für ATmega168/AVRStudio. Beschränkt auf einfache Kommandos (up/down/stop/...)

### Kontakt

* [Fehler/Bug-Meldungen, Feature-Requests](https://github.com/zwiebert/tronferno-mcu/issues) (Englisch,Deutsch).
* Project-Autor und Wartung: Bert Winkelmann <tf.zwiebert@online.de> (Deutsch,Englisch)
