
### Tronferno Konfiguration über Kommandozeilenoptionen


#### Konfiguration über Kommandozeilen-Terminal
  * Terminal kann über USB oder telnet genutzt werden
  * Kommandozeilen bei telnet zusätzlich mit Enter-Taste abschließen
  * Für USB ein Terminalprogramm nutzen wie GTKTerm, ...
  * Für Telnet muss eine Netzwerkverbindung bestehen und du musst die IP4-Adresse kennen. Der Port ist 7777.
    <pre>telnet 192.168.1.69 7777</pre>
  * Optionen mit "config" Kommando auslesen oder setzen
      * Beispiel: Option network anzeigen <pre>config network=?;</pre>
      * Beispiel: Option network auf "wlan" einstellen <pre>config network=wlan;</pre>

#### Konfiguration über Menutool
  * Das Tool erlaubt neben dem Flashen der Firmware die Einrichtung des Netzwerks. Weitere Konfiguration ist darüber nicht möglich.
  * Download [Firmware-ZIP-Archiv](https://codeload.github.com/zwiebert/tronferno-mcu-bin/zip/refs/heads/master) auf einen Linux- oder Windows-PC
  * Bei Bedarf [Menutool Dokumentation](https://github.com/zwiebert/tronferno-mcu-bin/blob/beta/docs/menutool.md) lesen
  * Starte menutool.sh (Linux) oder menutool.cmd (Windows)
  * Drücke "p". Nun wähle USB port an welchem der ESP32 mit dem PC verbunden ist 
  * Drücke "c". Konfigure die Kommandozeilenoptionen. Am Ende verlasse das Untermenu mit "y".
  * Drücke "o". Die konfigurierten Optionen werden nun über USB gesendet.


#### Konfiguration über FHEM-HomeServer
  * Das FHEM-Modul erlaubt nebem dem Flashen der Firmware den Zugriff auf ausgewählte Kommandozeilenoptionen.
  * Den Microcontroller über USB mit FHEM-HomeServer verbinden
  * [Tronferno-Module](https://github.com/zwiebert/tronferno-fhem) auf dem FHEM-Server installieren über folgendes Kommando:
     * <pre>update all ht<span>tps://raw.githubusercontent.com/zwiebert/tronferno-fhem/master/modules/tronferno/controls_tronferno.txt</pre>
  * Kommandozeilenoptionen haben den Prefix "mcc-" und werden mit "set" angesprochen (z.B. "set tfmcu mcc-network wlan").
  * In FHEMWEB sind die Optionen im "set" Menü zu finden. Eine Hilfe für die ausgewählte Option wird darunter angezeigt
  