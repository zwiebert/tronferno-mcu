
### How to build on Linux and flash to ESP32

* Prepare Hardware:
  * ESP32 board connected to your PC-USB.
  * RF receiver connected to esp-GPIO number 17
  * RF transmitter connected to esp-GPIO number 22
  * End-position-Button connected to esp-GPIO number (TODO)


* Install esp-idf and set IDF_PATH environment variable (usually "~/esp/esp-idf")

* open a shell in sub-directory user/esp32:
  * run: make menuconfig  (configure MCU, if needed)
  * make flash        (flash MCU)
  * make monitor      (FIXME: this terminal has no local echo!)

* optional: pre-configure your private data in new file user/sensitive/defaults.h which should contain:
```
#define MY_FER_CENTRAL_UNIT_ID 0x80****
#define MY_MCU_ATMEGA328_BAUD_RATE 38400
#define MY_MCU_ESP8266_BAUD_RATE 115200
#define MY_GEO_LONGITUDE +13.****
#define MY_GEO_LATITUDE +52.****
#define MY_GEO_TZ CET-1CEST-2,M3.5.0,M10.5.0
#define MY_WIFI_SSID "*******"
#define MY_WIFI_PASSWORD "************"
```

* build and flash on Linux shell from main directory


        make esp32-flash       #writes our firmware program

