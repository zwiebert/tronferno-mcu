SRC_BASE ?= ../..
CPPFLAGS += -DMCU_ESP8266 #-I app_config/proj_app_cfg.h

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

# components, modules, libs
COMP_DIR=src/components
COMPSHARE_DIR=components-mcu/components
COMPSHARE_PATH := $(SRC_BASE)/$(COMPSHARE_DIR)
COMP_PATH := $(SRC_BASE)/$(COMP_DIR)

COMPSHARE = txtio net debug cli key_value_store misc misc_time userio config_kvs net_mqtt net_http_server storage storage/esp8266/spiffs/src  net_mqtt/esp8266/esp_mqtt/mqtt
COMPONENTS = gpio config cli_app fernotron  fernotron_txtio  fernotron_sep fernotron_cuas fernotron_alias fernotron_pos fernotron_auto userio_app app app_config net_mqtt_app 
MODULES	=  src/esp8266 src/esp8266/driver src/lib/time
LIBS   =  gcc hal phy pp net80211 lwip wpa main crypto 
EXTRA_INCDIR += . 
#spiffs
EXTRA_INCDIR += $(addprefix $(COMPSHARE_PATH)/storage/,esp8266/spiffs/src include/storage/esp8266)
#time
EXTRA_INCDIR += $(SRC_BASE)/src/lib/time
# esp_mqtt
EXTRA_INCDIR += $(addprefix $(COMPSHARE_PATH)/net_mqtt/esp8266/,esp_mqtt/mqtt/include esp_mqtt/include .)
CPPFLAGS += -DLOCAL_CONFIG_AVAILABLE # for esp_mqtt
LIBS += mbedtls

define GEN_COMPONENTS
EXTRA_INCDIR += $(SRC_BASE)/$(1)/$(2)/include $(SRC_BASE)/$(1)/$(2)/include/$(2)
MODULES += $(1)/$(2)
ifneq ("$(wildcard $(SRC_BASE)/$(1)/$(2)/esp8266)","")
MODULES += $(1)/$(2)/esp8266
endif
endef
$(foreach component,$(COMPSHARE),$(eval $(call GEN_COMPONENTS,$(COMPSHARE_DIR),$(component))))
$(foreach component,$(COMPONENTS),$(eval $(call GEN_COMPONENTS,$(COMP_DIR),$(component))))


ifeq (1,0)
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

## make irom archive (move functions from RAM to Flash-ROM on ESP8266)
%irom.a : %.a
	$(Q) $(OBJCOPY) --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal --rename-section .iram0.text=.text  $(<) $(@);


include $(SRC_BASE)/src/esp8266/common.mk


libmirom: $(LIBMIROM_A)

