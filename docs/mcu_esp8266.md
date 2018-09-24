
### How to build on Linux or Windows and flash to ESP8266

* Prepare Hardware:
  * Esp8266 board connected to your PC-USB.
  * RF receiver connected to esp-GPIO5 (= pin D1 on mini boards)
  * RF transmitter connected to esp-GPIO4 (= pin D2 on mini boards)


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
#define MY_GEO_LONGITUDE +13.****
#define MY_GEO_LATITUDE +52.****
#define MY_GEO_TIMEZONE +1.0
#define MY_GEO_DST dstEU
#define MY_WIFI_SSID "*******"
#define MY_WIFI_PASSWORD "************"
```

* build and flash on Linux shell

	make esp8266-erase       #optional. erases flash.  any configuration data and stored files are lost
        make esp8266-flashinit   #required after erase. rewrites some calibration and blank data (can fix a boot-loop)
        make esp8266-flash       #writes our firmware program
    
 *  On Windows Commandline Unofficial Development Kit for Espressif ESP8266:
   
        make WINDOWS=1 ESPPORT=comX esp8266-flashinit esp8266-flash

