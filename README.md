# tronferno-mcu

  Tronferno is a firmware to turn an ESP32 MCU board into an hardware
  dongle to control Fernotron devices (usually shutters).

  It aims to provide all functionality of the original programming
  central 2411, but it also can do just plain up/down/stop commands,
  if you want just that.

  * Command interfaces: [CLI](docs/CLI.md) at USB or TCP/IP, [Webserver](docs/webserver.md) at [HTTP](docs/http.md), MQTT

  * Supported by FHEM home server via specific module for USB
    connection

  * Can be integrated into homer servers via its MQTT interface

  * Android App available


## Required Hardware

See the [hardware](hardware.md) document.


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
 * 2020: shutter positions in percent implemented
 * 2018: Moving to ESP32/esp-idf
 * 2017: Moving to ESP8266/NonOS
 * 2017: Moved to ATmega328p. Support for timer programming.
 * 2011: First version on ATmega168/AVRStudio. Limited to plain commands up/down/stop/...

### Project Author

Bert Winkelmann <tf.zwiebert@online.de>
