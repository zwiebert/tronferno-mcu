#### MCU boards

##### ESP32

  * Recommended: ESP32, 4MB FLASH, WLAN
  * Note: the optional Ethernet functionality is implemented different on different boards (may even be different on different revisions of the same board).
  * Ethernet is working and tested on:
       * Olimex ESP32-Gateway Rev. E
  * SDK/toolchain: esp-idf / xtensa-esp32-elf

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