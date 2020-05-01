#### MCU boards

##### ESP32

  * Recommended: ESP32, 4MB FLASH, WLAN
  * Note: the optional Ethernet functionality is implemented different on different boards (may even be different on different revisions of the same board).
  * Ethernet is working and tested on:
       * Olimex ESP32-Gateway Rev. E
  * SDK/toolchain: esp-idf / xtensa-esp32-elf


##### ESP8266
  * Still supported but in comparison to ESP32 its is missing HTTP server, firmware OTA, Ethernet, WIFI-accces-point.  
  * Tested on: ESP8266 board with 4MB flash-ROM.
  * SDK/Toolchain: ESP8266_NONOS_SDK (+ libm.a from esp-open-sdk) / xtensa-lx106-elf



##### AVR ATMEGA328P (on "Nano" board)
  * only basic functionality
  * firmware compiles (but too big to load) with: git checkout 80043d2e0bd5bd94be9053207e2fcf5aea391c71
  * firware compiles and runs fine with: git checkout e29af9767c6492f66b1fe99637737d14fd82d5b9
  * SDK/toolchain: avr-libc, avr-gcc, avrispII programmer hardware with avrdude

#### Additional Hardware

  * 433 MHz RF transmitter (like FS1000A) to send commands. Data pin connected to GPIO of MCU

  * Optional: 433 MHz RF receiver (like RXB6) to receive commands. Data pin connected to GPIO of MCU. 
         * Please note, a super-regeneration receiver can generate noise and disturb other receivers. Better use a super-heterodyne receiver, like the RXB6

  * Optional: Button-Switch (normally open) connected to GND and GPIO of MCU.


#### More Info
  * [GPIO pin numbers](pins.md)
  * [Schematic](schematic.pdf)

<hr>

![Schematic](img/schematic.png)