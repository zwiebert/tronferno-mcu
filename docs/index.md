|Change Language: [Deutsch](index-de.md), English|

# Programming central, repeater and network hub for Fernotron


The tronferno-mcu firmware can do the things the original Fernotron 2411 programming central and 2427 repeater can do, but adds some modern features like a web interface and home-server integration.


### Documentation

If you want to learn more, here are links to some topics:

  * [A list of features](features.md)
  * [How you make it all work](starter.md)
  * [FAQ list](starter_faq-de.md)


## Repositories


 * [tronferno-mcu](https://github.com/zwiebert/tronferno-mcu) Source distribution for developers

 * [tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin): Firmware binaries, tools and docs for end users


 * [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem): Optional module for integration into home server FHEM. But just using MQTT works fine too.



### History
 * 2022: WebGUI finished. All main features implemented.
 * 2021: Port WebGUI to Svelte/Tailwind
 * 2020: shutter positions implemented
 * 2018: Moving to ESP32/esp-idf
 * 2017: Moving to ESP8266/NonOS
 * 2017: Moved to ATmega328p. Support for timer programming.
 * 2011: First version on ATmega168/AVRStudio. Limited to plain commands up/down/stop/...

### Contact 

* Project-Author and maintainer: Bert Winkelmann <tf.zwiebert@online.de>
* To report bugs you can also open an issue on GitHub