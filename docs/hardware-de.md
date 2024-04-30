|Change Language: Deutsch, [English](hardware.md)|

## Hardware

#### Mikrocontroller-Board

  * Empfohlen: Board mit ESP32-WROOM, 4MB FLASH, WLAN, Optional: Ethernet mit PHY8710/8720 Chip.
  * LAN/Ethernet wird von verschiedenen Board (und Boardrevisionen) verschieden implementiert
  * LAN/Ethernet wurde getestet mit dem folgenden Board:
       * Olimex ESP32-Gateway Rev. E (Revision G sollte auch funktionieren und ist die bessere Variante)

#### 433.92 MHz Funk-Sender und -Empfänger

  Es wird entweder ein kombiniertes Sende/Empfangsmodul benötigt oder alternativ dazu je ein Modul zum Senden und Empfangen.

  * Kombiniertes Sende- und Empfangsmodul
  
     * CC1101 433MHz Modul
         * Es werden neben Senden (GDO0) und Empfangen (GDO2) vier zusätzliche GPIOs belegt für das SPI-Interface (MISO,MOSI,SCK,CSN) welches den CC1101 steuert.
         * Alle sechs für den CC1101 benötigten GPIOs sind in der Weboberfläche frei konfiguriertbar und die der CC1101 ist dann an diese GPIOs zu verdrahten.
         * Der ESP32 kann beliebige GPIOs für SPI verwenden. Nur für performance-kritische Anwendungen muss man die speziellen SPI-GPIOs verwenden. Für den CC1101 unötig.
         * Benutzt man doch die GPIOs des SPI2-Interfaces des ESP32, wird der ESP32 nicht mehr booten, falls er 3.3 Volt Flash-Speicher verbaut hat.
            * Problemursache ist GPIO12. Er hat eine Doppelfunktion (Bootstrapping-Pin) zur Auswahl der korreten Spannung (1.8 oder 3.3 Volt) des Flash-Speichers.
              Der Anschluss an den CC1101 würde verhindern, dass der Flash mit 3.3V laufen kann. Wenn er 1.8V braucht, gibt es keine Probleme.
            * Problemlösung:
                1. Den GPIO12 vermeiden und andere GPIOs einstellen:
                    * CC1101 beliebig: MISO=GPIOxx, MOSI=GPIOxx, SCLK/SCK=GPIOxx, CS0/CSN=GPIOxx, GDO0=GPIOxx, GDO2=GPIxx 
                2. Spannung des Flash-Speichers dauerhaft auf die korrekten 3.3 Volt einstellen, mit dem efuse Programm 
                    * Warnung: Einstellungen mit efuse sind nur einmalig möglich und endgültig. Man sollte sicher wissen welche Voltzahl korrekt ist.
                    * `esp-idf/components/esptool_py/esptool/espefuse.py set_flash_voltage 3.3V`
                    * Der Flash Speicher läuft nun fest mit dieser Voltzahl
                    * CC1101 an SPI2: MISO=GPIO12, MOSI=GPIO13, SCLK/SCK=GPIO14, CS0/CSN=GPIO15, GDO0=GPIOxx, GDO2=GPIxx 

  * Reines Sendemodul
     * FS100A: getestet und funktioniert, aber nicht empfohlen
        * Benötigt mehr als 5 Volt für gute Reichweite
        * Es wird ein GPIO-Pin benötigt der in der Weboberfläche ausgewählt werden kann    
     * WL102: getestet und funktioniert.
        * Es kann mit den 3.3 Volt gespeist werden, welche das ESP32 Board bereitstellt.
        * Es wird ein GPIO-Pin benötigt der in der Weboberfläche ausgewählt werden kann
     
  * Reines Empfangsmodul   
     * RXB6: getestet und funktioniert.
        * Es kann mit 5V gespeist werden, welche das ESP32 Board bereitstellt
        * Es wird dafür ein GPI-Pin benötigt der in der Weboberfläche ausgewählt werden kann.  
     * RX470C-V01: getestet und funktioniert.
        * Es kann mit 3.3V (2.2 ... 5V) gespeist werden, welche das ESP32 Board bereitstellt
        * Es wird dafür ein GPI-Pin benötigt der in der Weboberfläche ausgewählt werden kann.
      

#### Hand-Taster
   * Der Handtaster ist dazu gedacht, die Endpunkte eines Rollladen-Motors zu justieren. Es wird dafür ein GPI-Pin benötigt der in der Weboberfläche ausgewählt werden kann. Der andere Anschluss wird mit Masse verbunden.


<hr>

![Schematic](img/schematic.png)