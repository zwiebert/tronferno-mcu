# tronferno-mcu

MCU firmware to control Fernotron devices via CLI . Can run on micro controller ESP32, ESP8266. (ATMEGA328P is still there, but currently the generated firmware is to large).

#### This project is Experimental Software. It may contain bugs and errors. Use at your own risk.


### Hardware Requirements
 - MCU board
 - 443MHz RF transmitter (like FS1000A)
 - 443 MHz RF receiver (like RXB6)

RF receiver is optional and you may only ever need it to sniff the ID of your original central unit once.  If you want to keep it connected to allow scann, don't use an inferior super-regeneration receiver. It can block the frequency by transmitting noise occasionally. If you want to leave the receiver connected, use a super-heterodyne receiver like the RXB6. 


### Download pre-built binary images to flash
   * go to the separate github repository [zwiebert/tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin)

### How to build and flash

See documentation in doc/mcu_*.md
    
      
### Recent Changes

 * 2018-09: esp32 support added
 * 2018-05: bugfix: astro function was totally off (sorry)
 * 2018-02: bugfix: rolling counter was no longer updated (shutter ignores commands)
 * 2017-11: allow adjusting end positions (needs hardware button) (send option SEP)
 * 2017-11: finally implemented repeated sending for plain commands. (send option r)
 * 2017-11: print now debug info after mcu crash like described in the manual. was really helpful with finding that random crash introduced recently
 * 2017-11: added SPIFFS for later use. Currently disabled in user_config.h
 * 2017-11: persistent data will no longer be erased by updating the firmware. (used to be stored with the irom segment, now its separate)
 * 2017-11: CLI: backspace editing now works. Also quoting strings (like strange-password="ab;c d";). And most boolean options default to 1 now (can write now: sun-auto instead sun-auto=1)
 
### Problems
 * I'm not 100% sure about how the astro data is organized. So the times may be a bit off
 * The function of some bytes in the last line of the timer programming data is still unknown to me.

### History
I started this project in 2011 from scratch using an ATmega168. Sending normal commands like "up" and "down" was not to hard to figure out. But I gave up on programming the timers and the project went on hiatus. In 2017 I finally figured out the timer programming.  Had to switch to an ATmega328p for enough flash memory, and then to the ESP8266, for its built-in WIFI. 

### Related Projects
Its all too complicated and totally useless?
  * [GUI for Android](https://github.com/zwiebert/tronferno-andro). It kinda works already.
  * FHEM module [10_Tronferno.pm](https://github.com/zwiebert/tronferno-fhem) to send plain commands via TCP to tronferno-mcu
  * FHEM module [10_Fernotron.pm](https://github.com/zwiebert/tronferno-fhem) to send plain commands via SIGNALduino-IODev to Fernotron devices.
 
  
### Project Author

Bert Winkelmann <tf.zwiebert@online.de>

### All CLI commands

Look at the [CLI manual](https://github.com/zwiebert/tronferno-mcu/blob/masterdocs/CLI.md) in the docs folder.

