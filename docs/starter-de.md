##Tronferno Schnellstart-Anleitung


####Controller

Ein ESP32 Board wird benötigt. Die Firmware kann mittels Linux- oder Windows-PC oder auch mittels FHEM-Server installiert werden:
  
#####Linux-PC
1. python und pyserial müssen auf dem Linux-PC installiert sein
* Download und Entpacken des [Firmware-Binary-Archivs](https://codeload.github.com/zwiebert/tronferno-mcu-bin/zip/master) auf den PC 
* Flashen und Konfigurieren mit dem Programm menutool.sh (Anleitung im dazugehörigen [README.md](https://github.com/zwiebert/tronferno-mcu-bin/blob/master/README.md))
   
#####Windows-PC
1. Download und Entpacken des Firmware-Binary-Archivs auf den PC (es wird keine weitere Software benötigt)
* Flashen und Konfigurieren mit dem Programm menutool.cmd (Anleitung im dazugehörigen [README.md](https://github.com/zwiebert/tronferno-mcu-bin/blob/master/README.md))
        
#####FHEM-Server
1. python und pyserial müssen auf dem FHEM-Server installiert sein
* Anschließen eines ESP32-Modules per USB an den FHEM-ServerServer
* Installieren und Definieren des [TronfernoMCU-Moduls](https://github.com/zwiebert/tronferno-fhem/blob/master/README-de.md) auf dem FHEM Server, durch Eingabe unten stehender Befehle in FHEMWEB (der FHEM Weboberfläche)
* In FHEMWEB das tfmcu Gerät öffnen und den Set-Menüpunkt "mcu-firmware.esp32" öffnen. Dort "upgrade" auswählen und abschließend auf "set" klicken.

           update all https://raw.githubusercontent.com/zwiebert/tronferno-fhem/master/modules/tronferno/controls_tronferno.txt
          ...warten...
          shutdown restart
          define tfmcu TronfernoMCU /dev/ttyUSBx 
          set tfmcu mcu-firmware.esp32 upgrade

####Funkmodule

Fernotron sendet und empfängt auf 443MHz.
        
* Ein 443 MHz Sender wird benötigt um Fernotron Geräte zu steuern. Anschluss an GPIO_22  
* Ein 443 MHz Empfänger ist optional: Anschluss an GPIO_17. Damit können Fernotron-Sender empfangen werden.
  
  
####Ansteuerung
  
Der ESP32-Controller je nach Bedarf durch verschiedene Geräte bedient werden
  
#####Web-Browser (unfertig)
1. Der in der Firmware integrierte Web-Server muss zuerst mit http-enable=1 aktiviert werden (z.B. mit dem menutool aus dem Firmware-ZIP)
* IP4 Adresse des ESP32 in Broswer-Adressleiste eingeben.
        
     
#####FHEM-Homeserver (USB oder TCP/IP)
* GitHub: [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem)
     
#####Homeserver die MQTT unterstützen (z.B. OpenHAB)
* MQTT muss dazu (z.B. mit dem menutool) aktiviert und konfiguriert werden
* [weitere Infos](https://github.com/zwiebert/tronferno-mcu-bin/blob/master/README.md)
     
#####Android-App Tronferno
* GitHub: [tronferno-andro](https://github.com/zwiebert/tronferno-andro)
           
#####Terminal-Programm (USB oder TCP/IP)
* Lokal-Echo sollte im Terminalprogramm aktiviert werden um die eigenen Eingaben sehen zu können
     
     


