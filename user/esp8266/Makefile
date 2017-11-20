# Main settings includes
include	 ./user/esp8266/settings.mk

# Individual project settings (Optional)
#BOOT		= new
#APP		= 1
#SPI_SPEED	= 40
#SPI_MODE	= QIO
SPI_SIZE_MAP	= 4
#ESPPORT		= /dev/ttyUSB0
#ESPBAUD		= 115200

#V=1
# Basic project settings
MODULES	=   user/main user/esp8266 user/esp8266/driver user/time
LIBS	= c  gcc hal phy pp net80211 lwip wpa main crypto

# Root includes
#include	C:/Espressif/examples/ESP8266/common_nonos.mk



 .PHONY: libmirom force

## libm, libmirom
LIBM_A := $(HOME)/esp/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/libm.a
LIBMIROM_A :=  $(HOME)/esp/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/libmirom.a
LIBS += mirom
ifeq ($(WINDOWS),1)
#GEN_LIBS +=
CPPFLAGS += -DFER_RECEIVER_MINIMAL
else
GEN_LIBS += $(LIBMIROM_A)
endif

$(LIBMIROM_A) : $(LIBM_A)
	$(OBJCOPY) --rename-section .text=.irom0.text \
		--rename-section .literal=.irom0.literal $(<) $(@);
## end libm


## spiffs.a, spiffsirom.a
LIBSPIFFS_A := spiffs/build/libspiffs.a
LIBSPIFFSIROM_A := spiffs/build/libspiffsirom.a
SPIFFS_AR += $(LIBSPIFFSIROM_A) 
EXTRA_INCDIR += spiffs/src user/esp8266
GEN_LIBS += $(LIBSPIFFSIROM_A)
CPPFLAGS += -DCONFIG_BUILD_SPIFFS 

$(LIBSPIFFSIROM_A) : $(LIBSPIFFS_A)
	$(OBJCOPY) --rename-section .text=.irom0.text \
		--rename-section .literal=.irom0.literal $(<) $(@);

SPIFFS_SRC := $(wildcard spiffs/src/*.c)
SPIFFS_OBJ := $(patsubst spiffs/src/%.c,spiffs/build/%.o,$(SPIFFS_SRC))

$(LIBSPIFFS_A) : $(SPIFFS_OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^
## end spiffs.a

	
	
include ./user/esp8266/common.mk

## rules for compiling spiffs
$(eval $(call compile-objects,spiffs/build/,spiffs/src/))
## end

libmirom: $(LIBMIROM_A)

	