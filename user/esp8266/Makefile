SRC_BASE ?= ../..
CPPFLAGS += -DMCU_ESP8266

# Main settings includes
include	 $(SRC_BASE)/user/esp8266/settings.mk

# Individual project settings (Optional)
#BOOT		= new
#APP		= 1
#SPI_SPEED	= 40
SPI_MODE	= DIO
SPI_SIZE_MAP	= 4
#ESPPORT		= /dev/ttyUSB0
#ESPBAUD		= 115200

#V=1
# Basic project settings
MODULES	=   user/main user/esp8266 user/esp8266/driver user/misc
LIBS	= c  gcc hal phy pp net80211 lwip wpa main crypto



 .PHONY: libmirom force

## libm, libmirom
LIBM_A := $(HOME)/esp/esp-open-sdk/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/lib/libm.a
LIBMIROM_A := $(BUILD_BASE)/libmirom.a
LIBS += mirom
GEN_LIBS += libmirom

ifeq ($(WINDOWS),1)
#GEN_LIBS +=
#CPPFLAGS += -DFER_RECEIVER_MINIMAL
else
GEN_LIBS += $(LIB)
endif

$(LIBMIROM_A) : $(LIBM_A)
	$(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $(<) $(@);
## end libm


## spiffs.a, spiffsirom.a
SPIFFS_SRC_DIR := $(SRC_BASE)/spiffs/src
SPIFFS_BUILD_DIR := $(BUILD_BASE)/spiffs/src

BUILD_DIRS += $(SPIFFS_BUILD_DIR)	
SPIFFS_A := $(SPIFFS_BUILD_DIR)/spiffs.a
SPIFFSIROM_A := $(SPIFFS_BUILD_DIR)/spiffsirom.a

SPIFFS_SRC := $(wildcard $(SPIFFS_SRC_DIR)/*.c)
SPIFFS_OBJ := $(patsubst %.c,$(SPIFFS_BUILD_DIR)/%.o,$(subst $(SPIFFS_SRC_DIR)/,,$(SPIFFS_SRC)))

EXTRA_AR += $(SPIFFSIROM_A) 
EXTRA_INCDIR += $(SRC_BASE)/spiffs/src  $(SRC_BASE)/user/esp8266
GEN_LIBS += $(SPIFFSIROM_A)
CPPFLAGS += -DCONFIG_BUILD_SPIFFS
SUBMODULES += spiffs/src

pso :
	echo $(SPIFFS_OBJ)

$(SPIFFS_A) : $(SPIFFS_OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^
## end spiffs.a


## time.a, timeirom.a
TIME_SRC_DIR := $(SRC_BASE)/user/time
TIME_BUILD_DIR := $(BUILD_BASE)/user/time

BUILD_DIRS += $(TIME_BUILD_DIR)
TIME_A := $(TIME_BUILD_DIR)/time.a
TIMEIROM_A := $(TIME_BUILD_DIR)/timeirom.a

TIME_C_SRC := $(wildcard  $(TIME_SRC_DIR)/*.c)
TIME_ASM_SRC := $(wildcard  $(TIME_SRC_DIR)/*.S)
TIME_OBJ := $(patsubst %.c,$(TIME_BUILD_DIR)/%.o,$(subst $(TIME_SRC_DIR)/,,$(TIME_C_SRC)))
TIME_OBJ += $(patsubst %.S,$(TIME_BUILD_DIR)/%.o,$(subst $(TIME_SRC_DIR)/,,$(TIME_ASM_SRC)))

EXTRA_AR += $(TIMEIROM_A) 
#EXTRA_INCDIR +=
GEN_LIBS += $(TIMEIROM_A)

$(TIME_A) : $(TIME_OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^
## end time.a


## gdbstub.a
ifeq ("$(DEBUG)","1")
GDBSTUB_BUILD_DIR := $(BUILD_BASE)/esp-gdbstub
BUILD_DIRS += $(GDBSTUB_BUILD_DIR)
GDBSTUB_A := $(GDBSTUB_BUILD_DIR)/gdbstub.a
GDBSTUBIROM_A := $(GDBSTUB_BUILD_DIR)/gdbstubirom.a

CPPFLAGS += -DDEBUG -DGDBSTUB_FREERTOS=0 -DENABLE_GDB=1 
CFLAGS +=  -Og -ggdb
LDFLAGS += -ggdb
SUBMODULES += esp-gdbstub

GDBSTUB_SRC := $(wildcard esp-gdbstub/*.[c|S])
GDBSTUB_OBJ := $(patsubst %.S,$(BUILD_BASE)/%.o,$(patsubst %.c,$(BUILD_BASE)/%.o,$(GDBSTUB_SRC)))

EXTRA_AR += $(GDBSTUB_A) 
#EXTRA_INCDIR +=
GEN_LIBS += $(GDBSTUB_A)

$(GDBSTUB_A) : $(GDBSTUB_OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^
endif
## end gdbstub.a



# fetch submodule #FIXME:
spiffs/src esp-gdbstub:
	git submodule update --init --recursive

## make irom archive (move functions from RAM to Flash-ROM on ESP8266)
%irom.a : %.a
	$(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $(<) $(@);


include $(SRC_BASE)/user/esp8266/common.mk


libmirom: $(LIBMIROM_A)


