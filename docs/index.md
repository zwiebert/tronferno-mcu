|Change Language: [Deutsch](index-de.md), English|

# Programming central, repeater and network hub for Fernotron installations

The tronferno-mcu firmware can do the things the original Fernotron 2411 programming central and 2427 repeater can do, but adds some modern features like a web interface and home-server integration.


### Documentation

  * [Features](features.md)
  * [How to make it work](starter.md)
  * [FAQ list](starter_faq-de.md)


## Repositories

 * [tronferno-mcu](https://github.com/zwiebert/tronferno-mcu) Source distribution for developers

 * [tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin): Firmware binaries, tools and docs for end users

 * [tronferno-fhem](https://github.com/zwiebert/tronferno-fhem): Optional module for integration into home server FHEM. But using MQTT instead is now first choice.



### Project-History

 * 2022: completing Web-App. All main features now accessible.
 * 2021: porting Web-App to Svelte/Tailwind from Javascript/CSS
 * 2020: implementing shutter positions
 * 2018: Moving to ESP32/esp-idf
 * 2017: Moving to ESP8266/NonOS
 * 2017: Moving to ATmega328p. Support for timer programming.
 * 2011: First version on ATmega168/AVRStudio. Limited to plain commands up/down/stop/...

### Contact 

* [Bug-Reports, Feature-Requests](https://github.com/zwiebert/tronferno-mcu/issues) (English,German).

* Project-Author and maintainer: Bert Winkelmann <tf.zwiebert@online.de> (German,English)