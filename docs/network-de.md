### Mit WLAN Access-Point verbinden für die erstmalige Konfiguration (oder zur Reparatur der Netzwerkverbindung)
* aktiv nach dem ersten flashen (und wenn die Verbindung zum konfigurierten Netzwerk fehlschlägt)
* Verbinden deinen Laptop oder Smartphone mit dem Tronferno-WLAN-Asccesspoint: SSID=tronferno, WLAN-Passwort: tronferno
* Nach dem Verbinden öffne die Tronferno-Weboberfläche [http://192.168.4.1](http://192.168.4.1) in einem Browser.
* Nun richte die Netzwerkverbindung über die Weboberfläche ein
* Vergesse dabei nicht, auch den <strong>Webserver zu aktivieren</strong>.

### Tronferno einrichten zum Verbinden mit WLAN Router
* Einrichten über Weboberfläche
  * Config->MCU->Netzwerk->Verbindung: "Existing WLAN"
  * Config->MCU->Netzwerk->WLAN-Station->SSID: Die SSID deines WLANs
  * Config->MCU->Netzwerk->WLAN-Station->Passwort: Das Passwort deines WLANs
  * Config->MCU->Netzwerk->Webserver: Webserver hier einschalten um die Weboberfläche später nutzen zu können
* Einrichten über [Kommandozeilenoptionen](mcu_config_cli-de.md)
  * <pre>config network=wlan wlan-user=XXX wlan-password=XXX http-enable=1;</pre>

### Tronferno einrichten zum verbinden mit LAN (Ethernet)
* Einrichten über Weboberfläche
  * Navigation: Config->MCU->Netzwerk
     * Verbindung: "Ethernet"
     * LAN/Ethernet->lan-phy: LAN8720 ist getestet (ungetestet: RTL8201, IP101)
     * LAN/Ethernet->Power-ON/OFF-GPIO: boardabhängig, dieser GPIO schaltet die Stromversorgung für Ethernet auf dem Board ein/aus.
     * Webserver->Ein/Aus: Webserver hier einschalten um die Weboberfläche später nutzen zu können
* Einrichten über [Kommandozeilenoptionen](mcu_config_cli-de.md)
  * <pre>config network=lan lan-phy=lan8720 lan-pwr-gpio=-1 http-enable=1;</pre>
  

### Webserver einrichten
  * Einrichten über Weboberfläche:
     * Navigation: Config->MCU->Netzwerk
         * Webserver->Ein/Aus: Webserver hier einschalten um die Weboberfläche später nutzen zu können
         * Webserver->Auth-Name, Webserver->Auth-Passwort: Optional: Wenn nicht leer, dann ist ein Login zur Nutzung der Weboberfläche nötig
  * Einrichten über [Kommandozeilenoptionen](mcu_config_cli-de.md)
     * <pre>config http-enable=1 http-user=XXX http-password=XXX;</pre>
 

  
#### Mehr Info
  * [MCU configuration](mcu_config.md)

  
------------

![ScreenShot](img/tfmcu_settings_net.png)