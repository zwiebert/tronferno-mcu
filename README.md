# tronferno-mcu
MCU firmware to control Fernotron devices via CLI . Runs on one ESP8266 or ATMEGA328P 

State: Experimental software.

* Prepare Hardware:
  * esp8266 (development board)
  * RF receiver 443 MHz (like RXB6) on Pin D1
  * RF transmitter 443 MHz (FS1000A) on Pin D2


Build on Linux and flash to esp8266


configure the Makefiles in user/esp8266/
* flash size (default: 4MB / 32 MBit)
* serial port (default: /dev/ttyUSB0)
* paths to esp8266 build tools and sdk

* optional: pre-configure your private in new file user/sensitive/defaults.h which should contain:

#define MY_FER_CENTRAL_UNIT_ID 0x80****

#define MY_MCU_ATMEGA328_BAUD_RATE 38400

#define MY_MCU_ESP8266_BAUD_RATE 115200

#define MY_GEO_LONGITUDE -13.****

#define MY_GEO_LATITUDE +52.****

#define MY_GEO_TIMEZONE +1.0

#define MY_GEO_DST dstEU

#define MY_WIFI_SSID "*******"

#define MY_WIFI_PASSWORD "************"


* build and flash
  $ make esp8266-flash

* optional: configure your private data if not already done above. 
  * connect a serial terminal 115200-8-N-1 with local echo enabled:
  
    $ gtkterm --port /dev/ttyUSB0 --speed 115200 --echo
    
  * type your commands in the terminal. ca command line ends with a semicolon (;):
  
      help;
      
      config wlan-ssid="xxx" wlan-password="xxx";
      
      config latitude="xxx" longitude="xxx" time-zone="+1.0" dst="euDst";
      
      config cu=auto;     (this needs an 443Mhz RF receiver connected)
      
* connect a TCP terminal (like "Serial WIFI Terminal" from android play store)

     * find out which IP4 address your ESP8266 got from your WLAN accesspoint (look into the web interface of the AP).
     
     * Connect to port 7777 on this IP4 address 
     


* try to send commands from serial port terminal or TCP termminal.  RF Transmitter 443Mhz should be connected:

     send g=1 m=1 c=down;    (would close the shutter number 1 in group 1
     
     send c=up;              (would open all shutters from all groups)
     
     
In case you no longer have working Fernotron central unit. You would start from scratch:
 
     config cu=80xxxx  (just make up an address x can be one of 0...9 a...f  (hexadecimal).  write this address down)
    
    * press the set-button on the shutter you want to be number 1 in group 1
    
    send g=1 m=1 c=stop;
    
    * press the set-button on the next shutter
    
    send g=1 m=2 c=stop;
    
    * ... continue until  you have added your new central unit to all shutters you want
    
    * instead of pressing the set-button on the shutter you can use the code written on the shutter motor 
    
    * send a=9xxxxx    (where xxxxx is the hexadecimal code)
    
 Building it for ATMEGA328P ist similar:
 
 * Prepare Hardware 
    * Arduino Nano Board (Clone)
    * RF Receiver on D7
    * RF Transmitter on D2
    
    
   
   
 Too complicated and useless? I currently work on a GUI for android to work on top of the command line interface.
 
 The latitude and longitude configuration is required by astro function. But its currently disabled and a predefined table is used instead. Can be changed in user/main/astro.c  Enable math_write_astro(). But the data it generates is probably wrong at the moment.
 
 
 

