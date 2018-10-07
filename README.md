# tronferno-mcu

MCU firmware to control Fernotron devices via CLI . Can run on micro controller ESP32, ESP8266. (ATMEGA328P is still there, but currently the generated firmware is to large).

#### This project is Experimental Software. It may contain bugs and errors. Use at your own risk.


### Hardware Requirements
 - MCU board
 - 443 MHz RF transmitter (like FS1000A)
 - 443 MHz RF receiver (like RXB6)

RF receiver is optional.  It can be used to sniff the ID of your original central unit (usually a one time thing). It can also receive commands of other receivers, to keep track if a shutter may currently be opened or closed.
Please note, a super-regeneration receiver may block the frequency by transmitting noise occasionally. This is the reason you should not place some 433 MHz receiver devices (like some power outlets) near your 433 MHz shutter.  If you want to leave the receiver connected, use a super-heterodyne receiver like the RXB6. 


### Download pre-built binary images to flash
   * go to the separate github repository [zwiebert/tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin)

### How to build and flash

See documentation in [docs](https://github.com/zwiebert/tronferno-mcu/blob/master/docs/)/mcu_*.md
    
      
### Recent Changes

 * 2018-09: try to guess current open/close-status of shutters  
 * 2018-09: esp32 support added

### Problems
 * The function of some bytes in the last line of the timer programming data is still unknown to me. 

### History
 * 2018: Moving to ESP32 for better TCP stability and more RAM.
 * 2017: Moving to ESP8266 for builtin WiFi and more RAM. 
 * 2017: Moved to ATmega328p for more FLASH-ROM. Added programming of built-in timers.
 * 2011: First version for ATmega168. Limited to plain commands (up/down/etc). Audacity and Emacs-Lisp was used to analyze data.

### Related Projects
  * [GUI for Android](https://github.com/zwiebert/tronferno-andro). It kinda works already.
  * FHEM module [10_Tronferno.pm](https://github.com/zwiebert/tronferno-fhem) to send plain commands via TronfernoMcu-IODev (TCP or USB) to tronferno-mcu
  * FHEM module [10_Fernotron.pm](https://github.com/zwiebert/tronferno-fhem) to send plain commands via SIGNALduino-IODev to Fernotron devices. No need for tronferno-mcu hardware.
 
  
### Project Author

Bert Winkelmann <tf.zwiebert@online.de>

### All CLI commands

Look at the [CLI manual](https://github.com/zwiebert/tronferno-mcu/blob/master/docs/CLI.md) in the docs folder.

