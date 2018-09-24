### How to build and flash using ATMega-328P
 
 * Prepare Hardware 
    * Arduino Nano Board (Clone)  connected to USB on your PC
    * ISP programmer hardware (like avrispII, etc) connected to USB on your PC
    * RF receiver connected to atmega-PD2 (= pin D2 on Nano)
    * RF transmitter connected to atmega-PB3 (= pin D11 on Nano)
    
  * To build and flash, open the Solution file in AVR Studio on Windows or use make on Linux:
  ```
       make atmega328-flash
       make atmega328-eeprom
  ```

### Command Examples

There is no network support for the ATMega. You can add an WLAN or Bluetooth adapter to the serial port. But it will not have NTP like the ESP8266 has.  So the ATmega has to be told about the clock via CLI.

    config rtc=ISO_DATE_AND_TIME;    (looks like 2017-12-31T23:45:00)

You should never use the "timer" command unless time and time-zone are set correctly, because *any* timer command will also update the built-in RTC of the receiver.
```
    timer daily=0700-;    (all shutters opens daily at seven o'clock ... also updates the built-in RTC)
``` 
Some more options. 

```
    timer g=1 m=2 daily=-0400 weekly=0700++++00900-+ astro=+30 sun-auto=1;   (explanation below)
 ```
 
Explanation: Shutter 2 of group 1, closes daily (or nightly) at four o'clock, opens monday til friday at nine o'clock, opens saturday and sunday at nine o'clock, closes thirty minutes after sunset, activate the sun automatic. Note: there are 2 minus signs per day (for up and down each) but only one plus sign (which copies the entire previous day)
 
The sun automatic is controlled by a wireless window sensor powered by solar cells. You have to set a position the shutters goes whenever he receives the "too much sun" signal from the sensor:

      send g=2 m=3 c=sun-inst;    (set position for shutter 3 of group 2)

To send the "too much sun" command:

      send g=2 m=3 c=sun-down;    (the shutter goes down to the "too much sun" position. note: it would not go up to this position, if already below it)
### How to build and flash using ATMega-328P
 
 * Prepare Hardware 
    * Arduino Nano Board (Clone)  connected to USB on your PC
    * ISP programmer hardware (like avrispII, etc) connected to USB on your PC
    * RF receiver connected to atmega-PD2 (= pin D2 on Nano)
    * RF transmitter connected to atmega-PB3 (= pin D11 on Nano)
    
  * To build and flash, open the Solution file in AVR Studio on Windows or use make on Linux:
  ```
       make atmega328-flash
       make atmega328-eeprom
  ```

### Command Examples

There is no network support for the ATMega. You can add an WLAN or Bluetooth adapter to the serial port. But it will not have NTP like the ESP8266 has.  So the ATmega has to be told about the clock via CLI.

    config rtc=ISO_DATE_AND_TIME;    (looks like 2017-12-31T23:45:00)

You should never use the "timer" command unless time and time-zone are set correctly, because *any* timer command will also update the built-in RTC of the receiver.
```
    timer daily=0700-;    (all shutters opens daily at seven o'clock ... also updates the built-in RTC)
``` 
Some more options. 

```
    timer g=1 m=2 daily=-0400 weekly=0700++++00900-+ astro=+30 sun-auto=1;   (explanation below)
 ```
 
Explanation: Shutter 2 of group 1, closes daily (or nightly) at four o'clock, opens monday til friday at nine o'clock, opens saturday and sunday at nine o'clock, closes thirty minutes after sunset, activate the sun automatic. Note: there are 2 minus signs per day (for up and down each) but only one plus sign (which copies the entire previous day)
 
The sun automatic is controlled by a wireless window sensor powered by solar cells. You have to set a position the shutters goes whenever he receives the "too much sun" signal from the sensor:

      send g=2 m=3 c=sun-inst;    (set position for shutter 3 of group 2)

To send the "too much sun" command:

      send g=2 m=3 c=sun-down;    (the shutter goes down to the "too much sun" position. note: it would not go up to this position, if already below it)
