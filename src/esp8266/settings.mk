
# build directory
BUILD_BASE	?= $(SRC_BASE)/esp8266-build

# firmware directory
FW_BASE		?= $(BUILD_BASE)/firmware

# name for the target project
TARGET		= app

ifeq ("$(WINDOWS)","1")
# base directory of the ESP8266 SDK package, absolute
SDK_BASE	?=  c:/Espressif/ESP8266_SDK
#SDK_BASE        ?=  c:/ESP8266_NONOS_SDK-2.1.0
SDK_TOOLS	?= c:/Espressif/utils/ESP8266

# Extra libs, include and ld file
EXTRA_BASE	?= c:/Espressif/extra

# esptool path and port
ESPTOOL		?=  $(SDK_TOOLS)/esptool.exe
ESPPORT		?= COM3
else
# base directory of the ESP8266 SDK package, absolute
SDK_BASE	?= $(HOME)/esp/ESP8266_NONOS_SDK
SDK_TOOLS	?= $(HOME)/esp/ESP8266_NONOS_SDK/tools

OSDK_LIB_DIR  ?= $(HOME)/esp/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/


# Extra libs, include and ld file
EXTRA_BASE	?= $(HOME)/esp/ESP8266_NONOS_SDK/third_party

# esptool path and port
ESPTOOL		?= esptool.py
ESPPORT		?= /dev/ttyUSB0
endif

# Baud rate for programmer
ESPBAUD		?= 115200

ESPTOOL += --chip esp8266

# SPI_SPEED = 40, 26, 20, 80
SPI_SPEED ?= 40
# SPI_MODE: QIO, QOUT, DIO, DOUT
SPI_MODE ?= QIO
# SPI_SIZE_MAP
# 0 : 512 KB (256 KB + 256 KB)
# 1 : 256 KB
# 2 : 1024 KB (512 KB + 512 KB)
# 3 : 2048 KB (512 KB + 512 KB)
# 4 : 4096 KB (512 KB + 512 KB)
# 5 : 2048 KB (1024 KB + 1024 KB)
# 6 : 4096 KB (1024 KB + 1024 KB)
SPI_SIZE_MAP ?= 4
