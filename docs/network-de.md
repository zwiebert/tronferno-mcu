### WLAN Access-Point anlegen für die initiale Konfiguration
* aktiv nach dem ersten flashen
* SSID: tronferno
* Password: tronferno
* IP-Adresse: 192.168.4.1
* Webserver ist immer aktiviert
* CLI Option: config network=ap;

### Tronferno mit WLAN Router zu verbinden
* IP-Adresse: Zuweisung durch Router (DHCP)
* Login/Passwort: konfiguriert durch Nutzer
* Webserver: aktiviert durch Nutzer
* CLI Option: config network=wlan wlan-user=XXX wlan-password=XXX http-enable=1;



### Tronferno mit LAN (Ethernet) verbinden 
* IP-Adresse: Zuweisung durch Router (DHCP)
* Webserver: aktiviert durch Nutzer
* CLI Option: config network=lan http-enable=1;

  
#### Mehr Info
  * [MCU configuration](mcu_config.md)
  
