# tronferno-mcu

  Tronferno is a firmware to turn an ESP32 MCU board into a hardware
  dongle to control Fernotron devices (usually shutters).

  It aims to provide all functionality of the original programming
  central 2411, and more.
  
  It also tries to monitor the current position of the shutters if possible, so you can use voice commands like "alexa ... shutter bedroom 50%" succesfully most of the time and still be able to use a shutters internal timers (except random timer of course).
  
### Documentation
  * View [main documentation online](https://zwiebert.github.io/tronferno-mcu) 
  * View [main documentation offline](docs/index.md) 
  * Quickstart-Manual: [deutsch](docs/starter-de.md)
   

## Related Repositories

 * [tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin): Firmware binaries, tools and docs for end users

 * [tronferno-mcu](https://github.com/zwiebert/tronferno-mcu) Source distribution for developers

 * [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem): Perl module for integration into home server FHEM




### History
 * 2022: WebGUI finished. All main features implemented.
 * 2021: Port WebGUI to Svelte/Tailwind
 * 2020: shutter positions implemented
 * 2018: Moving to ESP32/esp-idf
 * 2017: Moving to ESP8266/NonOS
 * 2017: Moved to ATmega328p. Support for timer programming.
 * 2011: First version on ATmega168/AVRStudio. Limited to plain commands up/down/stop/...

### Project Author

Bert Winkelmann <tf.zwiebert@online.de>
