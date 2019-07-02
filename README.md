# tronferno-mcu



  Tronferno is a firmware to turn an ESP32 MCU board into an hardware
  dongle to control Fernotron devices (usually shutters).

  It aims to provide all functionality of the original programming
  central 2411, but it also can do just plain up/down/stop commands,
  if you want just that.

  * Command interfaces: USB, TCP, MQTT

  * Supported by FHEM home server via specific module for USB
    connection

  * Can be integrated into homer servers via its MQTT interface

  * Android App available


## Required Hardware

  * 433 MHz RF transmitter (like FS1000A) to send commands.

  * 433 MHz RF receiver (like RXB6) to receive commands.

  * ESP32, 4MB FLASH. (current main hardware for further development)

    * or: ESP8266, 4MB FLASH. (deprecated, no MQTT support for now(?))

    * or: ATMEGA328. (outdated firmware with limited features. No WLAN.)
      * firmware compiles (but too big to load) with: git checkout 80043d2e0bd5bd94be9053207e2fcf5aea391c71
      * firware compiles and runs fine with: git checkout e29af9767c6492f66b1fe99637737d14fd82d5b9


(Please note, a super-regeneration receiver can disturb other
receivers. This is the reason you should not place some 433 MHz power plugs near your 433 MHz shutter. A
super-heterodyne receiver, like the RXB6, does not have this issue)


## Related Repositories

 * [tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin): Firmware binaries, tools and docs for end users

 * [tronferno-mcu](https://github.com/zwiebert/tronferno-mcu) Source distribution for developers

 * [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem): Perl module for integration into home server FHEM

 * [tronferno-andro](https://github.com/zwiebert/tronferno-andro): Android App


### Documentation

  * Quickstart-Manual: [deutsch](docs/starter-de.md)

  * [CLI reference manual](https://github.com/zwiebert/tronferno-mcu/blob/master/docs/CLI.md) in the docs folder.

  * Buld/Flash information: [docs](https://github.com/zwiebert/tronferno-mcu/blob/master/docs/)/mcu_*.md


### History
 * 2018: Moving to ESP32
 * 2017: Moving to ESP8266
 * 2017: Moved to ATmega328p. Support for timer programming.
 * 2011: First version on ATmega168. Limited to plain commands up/down/stop/...

### Project Author

Bert Winkelmann <tf.zwiebert@online.de>
