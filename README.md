# tronferno-mcu
MCU firmware to control Fernotron devices via CLI . Runs on one ESP8266 or ATMEGA328P 

State: Experimental software.

* Prepare Hardware:
  * esp8266 (development board)
  * RF receiver 443 MHz (like RXB6) on Pin D1
  * RF transmitter 443 MHz (FS1000A) on Pin D2

RF receiver is optional. Its needed to sniff the ID of your original central unit. After that, it should be disconnected again.
For the sniffing a super-regeneration receiver can be used. It can block the frequency by transmitting noise now and then. To avoid this, use a super-heterodyne receiver like the RXB6. 

Build on Linux and flash to esp8266


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

* build and flash

    $ make esp8266-flash

* optional: configure your private data if not already done above. 
  * connect a serial terminal 115200-8-N-1 with local echo enabled:
  
    $ gtkterm --port /dev/ttyUSB0 --speed 115200 --echo
    
  * type your commands in the terminal. Command line ends with a semicolon (;):
  ```
      help;
      config wlan-ssid="xxx" wlan-password="xxx"; 
      config latitude="xxx" longitude="xxx" time-zone="+1.0" dst="euDst";
      config cu=auto;     (this needs an 443Mhz RF receiver connected)
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
 
     config cu=80xxxx;  (just make up an address, x can be one of 0...9 a...f  (hexadecimal).  write this address down)
    
    * press the set-button on the shutter you want to be number 1 in group 1
    
         send g=1 m=1 c=stop;
    
    * press the set-button on the next shutter
    
         send g=1 m=2 c=stop;
    
    * ... continue until  you have added your new central unit to all shutters you want
    
    * instead of pressing the set-button on the shutter you can use the code written on the shutter motor 
    ```
        send a=9xxxxx c=set;  (where xxxxx is the hexadecimal code)
    ```
 Building it for ATMEGA328P ist similar:
 
 * Prepare Hardware 
    * Arduino Nano Board (Clone)
    * ISP programmer hardware (like avrispII, etc)
    * RF Receiver on D7
    * RF Transmitter on D2
    
  * To build and flash, open the Solution file in AVR Studio on Windows or use make on Linux:
  ```
      $ make atmega328-flash
      $ make atmega328-eeprom
  ```
     
There is no network support for the ATMega. You can add an WLAN or Bluetooth adapter to the serial port. There is NTP on the ESP8266.  The ATmega has to be told about the clock via CLI.

     config rtc=ISO_DATE_AND_TIME;    (looks like 2017-12-31T23:45:00)

You should not use the "timer" command, if the time and time-zone is not set correctly.  This would mess up the built-in real time clock of the shutter you address any timer command to:
```
      timer daily=0700-;    (all shutters opens every day at seven o'clock ... and also updates the built-in RTC)     
      timer g=1 m=2 daily=-0400 weekly=0700++++00900+ astro=+30 sun-auto=1;  (shutter 2 of group 1 closes daily at four o'clock, opens monday til friday at nine o'clock. closes thirty minutes after sunset, activate the sun automatic.
 ```
      
The sun automatic is controlled by a wireless window sensor powered by solar cells. You have to set a position the shutters goes whenever he receives the "too much sun" signal from the sensor.

I have bought a few of these sensors in the past, but now the fernotron series is long discontinued.  To set the "too much sun" position:


      send g=2 m=3 c=sun-inst;    (set position for shutter 3 of group 2)

To send the "too much sun" command:

      send g=2 m=3 c=sun-down;    (the shutter goes down to the "too much sun" position. note: it would not go up to this position, if already below it)
      
      
 
      

   
 Too complicated and totally useless? I currently work on a GUI for Android.  Maybe I also add a binary repository later, for the people (if any) who just want to flash their MCUs without having to install the build environment on their PC.
 
 
 The latitude and longitude configuration is required by astro function obviously. But its currently disabled and a predefined table is used instead. Can be changed in user/main/astro.c  Enable math_write_astro(). But the data it generates is probably wrong at the moment.
 
 Also, when using timer command: all data is overwritten in the shutter device.  A daily timer will also overwrite the weekly timer and all flags like sun-auto. Exception: if you only send the real time (rtc-only=1), no timer data or flags besides the DST flag are changed.
 

