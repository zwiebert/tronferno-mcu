## Tronferno-Firmware installieren
  
### Linux-PC

Voraussetzung: python und pyserial müssen auf dem Linux-PC installiert sein

1. Download und Entpacken des [Firmware-Binary-Archivs](https://codeload.github.com/zwiebert/tronferno-mcu-bin/zip/master) auf den PC 1
1. ESP32 Board per USB mit dem PC verbinden
1. Flashen und Konfigurieren mit dem Programm [menutool.sh](menutool-de.md)

   
### Windows-PC
1. Download und Entpacken des Firmware-Binary-Archivs auf den PC (es wird keine weitere Software benötigt)
1. ESP32 Board per USB mit dem PC verbinden
1. Windows sollte einen COM-Port für den ESP32 anlegen. Falls nicht, muss erst noch der passende [USB-Treiber](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html) installiert werden 
1. Flashen und Konfigurieren mit dem Programm [menutool.cmd](menutool-de.md)
### FHEM-Server

Voraussetzung: python und pyserial müssen auf dem FHEM-Server installiert sein

##### FHEM Module installieren

1. ESP32 Board per USB mit dem FHEM-ServerServer verbinden
2. Installieren und Definieren des [TronfernoMCU-Moduls](https://github.com/zwiebert/tronferno-fhem/blob/master/README-de.md) auf dem FHEM Server, durch Eingabe unten stehender Befehle in FHEMWEB (der FHEM Weboberfläche)

           update all https://raw.githubusercontent.com/zwiebert/tronferno-fhem/master/modules/tronferno/controls_tronferno.txt
          [...warten...]
          shutdown restart
          define tfmcu TronfernoMCU /dev/ttyUSBx
          
          
##### Firmware flashen

In FHEMWEB das tfmcu Gerät öffnen und den Set-Menüpunkt "mcu-firmware.esp32" öffnen. Dort "upgrade" auswählen und abschließend auf "set" klicken.

###### Master Version flashen (möglicherweise veraltet):

          set tfmcu mcu-firmware.esp32 upgrade

###### Letzte Beta Version (möglicherweise unstabil):

          set tfmcu mcu-firmware.esp32 upgrade-beta-version





