SRC_BASE ?= ../..
CPPFLAGS += -DMCU_ESP8266

# Main settings includes
include	 $(SRC_BASE)/src/esp8266/settings.mk

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
COMPONENTS = debug gpio key_value_store storage net config cli cli_app fernotron  fernotron_txtio  fernotron_sep fernotron_cuas fernotron_alias fernotron_pos fernotron_auto misc  txtio  userio userio_app app app_config
MODULES	=  src/esp8266 src/esp8266/driver
LIBS   =  gcc hal phy pp net80211 lwip wpa main crypto

define GEN_COMPONENTS
EXTRA_INCDIR += $(SRC_BASE)/$(1)/$(2)/include $(SRC_BASE)/$(1)/$(2)/include/$(2)
MODULES += $(1)/$(2)

ifneq ("$(wildcard $(SRC_BASE)/$(1)/$(2)/esp8266)","")
MODULES += $(1)/$(2)/esp8266
endif

endef
$(foreach component,$(COMPONENTS),$(eval $(call GEN_COMPONENTS,src/components,$(component))))

ifeq (1,1)
## libc, libcirom
LIBC_A := $(SDK_BASE)/lib/libc.a
LIBCIROM_A := $(BUILD_BASE)/libcirom.a
LIBS += cirom
GEN_LIBS += $(LIBCIROM_A)

$(LIBCIROM_A) : $(LIBC_A)
	$(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $(<) $(@);
else
LIBS += c
endif

 .PHONY: libmirom force

## libm, libmirom
LIBM_A := $(OSDK_LIB_DIR)/libm.a
LIBMIROM_A := $(BUILD_BASE)/libmirom.a
LIBS += mirom
GEN_LIBS += $(LIBMIROM_A)

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
EXTRA_INCDIR += $(SRC_BASE)/spiffs/src  $(SRC_BASE)/src/esp8266
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
TIME_SRC_DIR := $(SRC_BASE)/src/lib/time
TIME_BUILD_DIR := $(BUILD_BASE)/src/lib/time

BUILD_DIRS += $(TIME_BUILD_DIR)
TIME_A := $(TIME_BUILD_DIR)/time.a
TIMEIROM_A := $(TIME_BUILD_DIR)/timeirom.a

TIME_C_SRC := $(wildcard  $(TIME_SRC_DIR)/*.c)
TIME_ASM_SRC := $(wildcard  $(TIME_SRC_DIR)/*.S)
TIME_OBJ := $(patsubst %.c,$(TIME_BUILD_DIR)/%.o,$(subst $(TIME_SRC_DIR)/,,$(TIME_C_SRC)))
TIME_OBJ += $(patsubst %.S,$(TIME_BUILD_DIR)/%.o,$(subst $(TIME_SRC_DIR)/,,$(TIME_ASM_SRC)))

EXTRA_AR += $(TIMEIROM_A) 
EXTRA_INCDIR += $(TIME_SRC_DIR)
GEN_LIBS += $(TIMEIROM_A)

$(TIME_A) : $(TIME_OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^
## end time.a

# fetch submodule #FIXME:
spiffs/src:
	git submodule update --init --recursive

## make irom archive (move functions from RAM to Flash-ROM on ESP8266)
%irom.a : %.a
	$(Q) $(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal --rename-section .iram0.text=.text  $(<) $(@);


include $(SRC_BASE)/src/esp8266/common.mk


libmirom: $(LIBMIROM_A)


