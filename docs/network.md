### WLAN Access-Point
* Purpose: to do the first time basic configuration by web browser 
* active after initial flashing the firmware until you reconfigure "network" option
* SSID: tronferno
* Password: tronferno
* IP-address: 192.168.4.1
* Webserver is always enabled
* CLI: config network=ap;

### WLAN Station
* IP-address: DHCP
* Login/Password: by configuration
* Webserver: enabled/disabled by configuration
* CLI: config network=wlan http-enable=1;



### LAN/Ethernet
* IP-address: DHCP
* Login/Password: by configuration
* Webserver: enabled/disabled by configuration
* CLI: config network=lan http-enable=1;

  
#### More Info
  * [MCU configuration](mcu_config.md)
  
------------

![ScreenShot](img/tfmcu_settings_net.png)
