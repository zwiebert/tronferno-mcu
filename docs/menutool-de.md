|Change Language: Deutsch, [English](menutool.md)|

## tronferno-menutool

  * Das Tool dient zum interaktiven Flashen der Firmware und zum Konfigurieren der WLAN Verbindung
  
  1. Starte menutool (./menutool.sh auf Linux, .\menutool.cmd auf Windows) vom aupt-Verzeichnis.
     Es öffnet sich ein Menü das über Buchstaben-Tasten gesteuert wird.

```

Tronferno MCU Flash Assistent
--------------------------------------
[s] Speichern  [X] Beenden [L] Sprache
--------------------------------------


1) Verbindung zum Mikrocontroller:
    [i] Suche Chip an USB ports
    [I] Zeige Info über Chip (esp32@/dev/ttyUSB1):
    [f] Konfiguriere USB und Flash
    [t] Starte serielles Terminal (Miniterm)

2) Flashe dem Mikrocontroller (esp32@/dev/ttyUSB1):
    [E] Lösche Flash Speicher komplett
    [w] Flashe die Firmware

3) Verbinde den Mikrocontroller zum Netzwerk
    [c] Konfiguriere Verbindungsdaten (WLAN,MQTT)
    [o] Schreibe Verbindungsdaten zum Mikrocontroller (erst nach dem Flashen möglich)

4) Zugriff auf Webserver erlangen: IP-Address: 1.2.3.4, Hostname: tronferno
    [a] Hohle IP Adresse vom Chip
   URLs: http://1.2.3.4 -or- http://tronferno.fritz.box

```
  1)
     * Wähle Anzeigesprache Deutsch mit 'L' (shift-L)
     * Schließe Mikrocontroller über USB an
     * Drücke 'i' zum Ausprobieren welcher COM/USB-Port der richtige ist
     * Drücke 'f' zur Konfiguration
  
 Hinweis: Unter Windows kann es nötig sein den virtual-com-port Treiber für den auf dem Board verbauten
          USB-Chip downzuloaden. Öffne Windows-DeviceManager um den Chip zu identifizieren. 
```
Press key to choose menu item:
 [y] apply changes and leave menu
 [X] discard changes and leave menu

 [a] chip (esp8266)
 [b] flash-size (detect)
 [c] serial-baud (115200)
 [d] serial-port (/dev/ttyUSB0)

Enter value for chip (esp8266): ...esp32

```
  2)
    * Wenn du den Flash Speicher total löschen möchtest, dann 'e' drücken.
    * Zum flashen der Firmware, 'w' drücken.
    * Drücke 'c' zur Konfiguration von WLAN und HTTP
```
Press key to choose menu item:

 [y] apply changes and leave menu
 [X] discard changes and leave menu

 [a] wlan-password (xxx)
 [b] wlan-ssid (xxx)
 [c] http-enable (1)
 [d] http-user ()
 [e] http-password ()
 [f] mqtt-enable (1)
 [g] mqtt-url (mqtt://192.168.1.42:7777)
 [h] mqtt-user ()
 [i] mqtt-password ()

```
  3)
   Drücke 'o' to write WLAN and MQTT login data to the chip. 
  
  4)
   Drücke 'a' zu auslesen der IP adresse from the microcontroller and use a webrowser to connect to it.
   