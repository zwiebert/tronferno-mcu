# tronferno-mcu

MCU firmware to control Fernotron devices like shutter motors the way the original 2411 central unit does.  It has a command line user interface which can be fully accessed via USB terminal program. See below for Related Projects for an (optional) App for Android. There is also an FHEM module. 

Supported MCUs:

* ESP32: works best
* ESP8266: works, but has more bugs and less features
* ATMEGA328P: much less features ... and still the current version no longer fits into its small flash-ROM. (See the Makefile variable ATMEGA328_CO of the binary project for the last working git tag. The binary image there is built with it and works)



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
 * 2017: Moved to ATmega328p for more FLASH-ROM. Added programming of built-in timer after figuring out all these checksums.
 * 2011: First version for ATmega168. Limited to plain commands (up/down/etc). Audacity and Emacs-Lisp was used to analyze data.

### Related Projects
  * [GUI for Android](https://github.com/zwiebert/tronferno-andro). It kinda works already.
  * FHEM module [10_Tronferno.pm](https://github.com/zwiebert/tronferno-fhem) to send plain commands via TronfernoMcu-IODev (TCP or USB) to tronferno-mcu
  * FHEM module [10_Fernotron.pm](https://github.com/zwiebert/tronferno-fhem) to send plain commands via SIGNALduino-IODev to Fernotron devices. No need for tronferno-mcu hardware.
 
  
### Project Author

Bert Winkelmann <tf.zwiebert@online.de>

### All CLI commands

Look at the [CLI manual](https://github.com/zwiebert/tronferno-mcu/blob/master/docs/CLI.md) in the docs folder.

