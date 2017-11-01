# tronferno-mcu

MCU firmware to control Fernotron devices via CLI . Can run on ESP8266 or ATMEGA328P micro controller.

#### This project is Experimental Software. It may contain bugs and errors. Use at your own risk.


### Hardware Requirements
 - MCU board.
 - 443MHz RF transmitter (like FS1000A)
 - 443 MHz RF receiver (like RXB6)

RF receiver is optional and you may only ever need it to sniff the ID of your original central unit once.  If you want to keep it connected, don't use an inferior super-regeneration receiver. It can block the frequency by transmitting noise occasionally. If you want to leave the receiver connected, use a super-heterodyne receiver like the RXB6. 


### How to flash the ESP8266 from binary images
   * go to the separate github repository [zwiebert/tronferno-mcu-bin](https://github.com/zwiebert/tronferno-mcu-bin)

### How to build on Linux or Windows and flash to ESP-8266

* Prepare Hardware:
  * esp8266 (development board) connected to USB on your PC
  * RF receiver connected to D1 on the MCU board
  * RF transmitter connected to D2


configure the Makefiles in user/esp8266/
* flash size (default: 4MB / 32 MBit)
* serial port (default: /dev/ttyUSB0)
* paths to esp8266 build tools and sdk
* add the tools to your PATH

* optional: pre-configure your private data in new file user/sensitive/defaults.h which should contain:
```
#define MY_FER_CENTRAL_UNIT_ID 0x80****
#define MY_MCU_ATMEGA328_BAUD_RATE 38400
#define MY_MCU_ESP8266_BAUD_RATE 115200
#define MY_GEO_LONGITUDE -13.****
#define MY_GEO_LATITUDE +52.****
#define MY_GEO_TIMEZONE +1.0
#define MY_GEO_DST dstEU
#define MY_WIFI_SSID "*******"
#define MY_WIFI_PASSWORD "************"
```

* build and flash on Linux shell

        make esp8266-flashinit
        make esp8266-flash
    
 *  On Windows Commandline Unofficial Development Kit for Espressif ESP8266:
   
        make WINDOWS=1 ESPPORT=comX esp8266-flashinit esp8266-flash

* optional: configure your private data if not already done above. 
  * connect a serial terminal 115200-8-N-1 with local echo enabled:
  
        gtkterm --port /dev/ttyUSB0 --speed 115200 --echo
    
  * type your commands in the terminal. Command line ends with a semicolon (;). No editing possible:
  ```
      help;
      config wlan-ssid="xxx" wlan-password="xxx"; 
      config latitude=xxx longitude=xxx time-zone=+1.0 dst=eu;
      config cu=auto;     (this needs a 443Mhz RF receiver connected)
  ```    
* connect a TCP terminal (like "Serial WIFI Terminal" from android play store)

     * find out which IP4 address your ESP8266 got from your WLAN accesspoint (look into the web interface of the AP).
     
     * Connect to port 7777 on this IP4 address 
     


* try to send commands from serial port terminal or TCP termminal.  RF Transmitter 443Mhz should be connected:
```
     send g=1 m=1 c=down;    (would close the shutter number 1 in group 1   
     send c=up;              (would open all shutters from all groups)
```    
     
In case you no longer have working Fernotron central unit. You would start from scratch:
 
     config cu=80xxxx;  (just make up an address, x needs to be one of 0...9 a...f  (hex).  write it down)
    
    * press the set-button on the shutter you want to be number 1 in group 1, and type quickly:
    
         send g=1 m=1 c=stop;
    
    * press the set-button on the next shutter, and type quickly (or use copy/paste)
    
         send g=1 m=2 c=stop;
    
    * ... continue until  you have added your new central unit to all shutters you want
    
    * instead of pressing the set-button on the shutter you can use the code written on the shutter motor 
    ```
        send a=9xxxxx c=set;  (where xxxxx is the hexadecimal code)
    ```
 ### How to build and flash using ATMega-328P
 
 * Prepare Hardware 
    * Arduino Nano Board (Clone)  connected to USB on your PC
    * ISP programmer hardware (like avrispII, etc) connected to USB on your PC
    * RF Receiver connected to D7
    * RF Transmitter connected to D2
    
  * To build and flash, open the Solution file in AVR Studio on Windows or use make on Linux:
  ```
       make atmega328-flash
       make atmega328-eeprom
  ```

### Command Examples

There is no network support for the ATMega. You can add an WLAN or Bluetooth adapter to the serial port. But it will not add NTP like the ESP8266 has.  So the ATmega has to be told about the clock via CLI.

    config rtc=ISO_DATE_AND_TIME;    (looks like 2017-12-31T23:45:00)

You should never use the "timer" command, if the time and time-zone is not set correctly.  This would mess up the built-in real time clock of the shutter you address any timer command to:
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
      
      

### Problems
 * I'm not 100% sure about how the astro data is organized. So the times may be off
 * The function of a few bytes in the last line of the timer programming data is still unknown to me.
 * When building the ESP8266 firmware on windows, the IRAM segment may overflow. Uncomment FER_RECEIVER_MINIMAL in user/main/common.h to disable some code. 

### History
I started this project in 2011 from scratch using an ATmega168. Sending normal commands like "up" and "down" was not to hard to figure out. But I gave up on programming the timers and the project went on hiatus. In 2017 I finally figured out the timer programming.  Had to switch to an ATmega328p for enough flash memory, and then to the ESP8266, for its built-in WIFI.   

### Future
Its all too complicated and totally useless?
  * I work on a [GUI for Android](https://github.com/zwiebert/tronferno-andro).
  * FHEM module to send CLI commands to our MCU-CLI via TCP socket
  * Software-only FHEM module directly outputting to a 433MHz RF transmitter
  
### Project Author

Bert Winkelmann

### CLI options (output of help command)

```
syntax: command option=value ...;
commands are: send, config, dbg, timer, help, 
send options:
a=(0|SenderID) address of the sender or 0 for the configured CentralUnit
g=[0-7]  group number. 0 is for broadcast
m=[0-7]  group member. 0 is for broadcast all groups members
c=(up|down|stop|sun-down|sun-inst|set) command to send

config options:
cu=(CentralUnitID|auto)  like 80abcd. auto: press Stop key on central unit in the next 60 seconds
rtc=ISO_TIME_STRING  like 2017-12-31T23:59:59
baud=serial_baud_rate
wlan-ssid="your_wlan_ssid"
wlan-password="your_wlan_password"
longitude=N like -13.23452 (to calculate sunset)
latitude=N like +52.34234
time-zone=N like +1
dst=(eu|0|1) daylight saving time: automatic: eu=europe. manually: 0=off, 1=on
verbose=(0|1|2|3|4|5)  set text output verbosity level: 0 for none ... 5 for max)
dbg options:
print=(rtc|cu)

timer options:
daily=T T is like 0730- or 07302000 or -2000  for up 07:30 and/or down 20:00
weekly=TTTTTTT like weekly=0730-++++0900-+ (+ repeats the previous T) for up 07:30 Mon-Fri and up 09:00 Sat-Sun
astro=N This enables astro automatic. N is the offset to sunset in minutes. So astro=+60 closes the shutter 60 minutes after sunset
sun-auto=1  1 enables and 0 disables sun automatic
random=1 enables random automatic. shutter opens and closes at random times, so it looks like you are home when you are not
rtc-only=1  Update the built-in real time clock of the shutter. Don't change its programmed timers (and flags)

help options:
none

```
