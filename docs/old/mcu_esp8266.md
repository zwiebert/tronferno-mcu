
### How to build on Linux or Windows and flash to ESP8266

* Prepare Hardware:
  * Esp8266 board connected to your PC-USB.
  * RF receiver connected to esp-GPIO5 (= pin D1 on mini boards)
  * RF transmitter connected to esp-GPIO4 (= pin D2 on mini boards)

install: 
ESP8266_NONOS_SDK to ~/esp/
esp-open-sdk to ~/esp/   (for libm.a wich ist not in the official SDK)
esp-idf  (for the latest esptool.py, which works on both ESP32 and ESP8266)


configure the Makefiles in user/esp8266/
* flash size (default: 4MB / 32 MBit)
* serial port (default: /dev/ttyUSB0)
* paths to esp8266 build tools and sdk
* add the tools to your PATH

