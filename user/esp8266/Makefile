# Main settings includes
include	 ./user/esp8266/settings.mk

# Individual project settings (Optional)
#BOOT		= new
#APP		= 1
#SPI_SPEED	= 40
#SPI_MODE	= QIO
SPI_SIZE_MAP	= 4
ESPPORT		= /dev/ttyUSB0
#ESPBAUD		= 115200

#V=1
# Basic project settings
MODULES	=   user/main user/esp8266 user/esp8266/driver user/time
LIBS	= c  gcc hal phy pp net80211 lwip wpa main crypto
# Root includes
#include	C:/Espressif/examples/ESP8266/common_nonos.mk



 .PHONY: libmirom force


LIBM_A := $(HOME)/esp/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/libm.a
LIBMIROM_A :=  $(HOME)/esp/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/libmirom.a
LIBS += m

GEN_LIBS := $(LIBMIROM_A)


$(LIBMIROM_A) : $(LIBM_A) force
	$(OBJCOPY) --rename-section .text=.irom0.text \
		--rename-section .literal=.irom0.literal $(<) $(@);
  
  
  include ./user/esp8266/common.mk
  

 
 
 libmirom: $(LIBMIROM_A)