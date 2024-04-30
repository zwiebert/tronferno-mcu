|Change Language: [Deutsch](hardware-de.md), English|

## Hardware

#### MCU boards

##### ESP32

  * Recommended: ESP32, 4MB FLASH, WLAN
  * Note: the optional Ethernet functionality is implemented different on different boards (may even be different on different revisions of the same board).
  * Ethernet is working and tested on:
       * Olimex ESP32-Gateway Rev. E
  * SDK/toolchain: esp-idf / xtensa-esp32-elf

#### Additional Hardware

##### RF (radio) modules

  The radio module(s) use 433.92 MHz frequency. Modulation is AM (OOK). You can use a single transceiver module (transmitter and receiver combined), or separate transmitter and receiver modules. The (optional) receiver is mainly used for keeping track of shutter positions.

  Please note, a super-regeneration receiver can generate noise and disturb other receivers. Better use a super-heterodyne receiver


  Supported and tested RF modules are:
  1. Transceiver
     * C1101 433.92 MHz transceiver module "V2.0". It came complete with a big detachable antenna, not this helix coil shit.
     * C1101 433.92 MHz transceiver module "V1.0". The frequency on this is more like 433.89 MHz. Some capacitors on the board may be wrong. You can adjust the frequency in the CC1101 tab of the web-app to 433.95 to correct this, but it does not matter that much.

  2. Transmitter
     * WL102-341  works fine with just the standard 3.3 V

  3. Receiver
     * RXB6 433.92 MHz super-heterodyne receiver, worked fine
     * RX470-4 - (I would not use the helix coil antenna again they sell with it)
     * AM-RRQ3-433P SUPERHETERODYNE COMPACT RECEIVER MODULE (not tested yet, but its in the original Fernotron 2427 repeater...)

  4. Stay away
     * FS100A transmitter (needs over-volting for acceptable range. With 5V the range was relatively poor). The usually bundled receiver is not great.

Before buying you may check other information sources. I only tested a few modules and there may be newer/better ones out there.

##### Other hardware
  * Optional: Button-Switch (normally open) connected to GND and GPIO of MCU.


#### More Info
  * [Schematic](schematic.pdf)

<hr>

![Schematic](img/schematic.png)
