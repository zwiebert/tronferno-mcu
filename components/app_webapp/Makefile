#Build webapp with node.js

MCU_IP_ADDR ?= 192.168.1.69
PROXY_TCP_PORT ?= 3000




THIS_ROOT := $(realpath .)

ifeq "x$(BUILD_DIR)" "x"
   BUILD_DIR ?= $(realpath .)/build
   JS_BLD_DIR ?= $(THIS_ROOT)/njs/build
   JS_BLD_DIR_DEV ?= /tmp/tronferno-mcu/njs/build
else
   JS_BLD_DIR ?= $(BUILD_DIR)/njs/build
   JS_BLD_DIR_DEV ?= /tmp/tronferno-mcu/njs/build
endif

JS_SRC_DIR := $(THIS_ROOT)/njs/src
NJS_DIR :=$(THIS_ROOT)/njs


SDKCONFIG_JSON = $(BUILD_DIR)/../config/sdkconfig.json

ifneq ("$(wildcard $(SDKCONFIG_JSON))","")
export SDKCONFIG_JS_DIR = $(BUILD_DIR)
SDKCONFIG_JS = $(SDKCONFIG_JS_DIR)/sdkconfig.js

$(SDKCONFIG_JS): FORCE
	(echo "export const sdkconfig = "; cat $(SDKCONFIG_JSON); echo ";") > $(SDKCONFIG_JS)
	touch -r $(SDKCONFIG_JSON) $(SDKCONFIG_JS)
else
export SDKCONFIG_JS_DIR = $(NJS_DIR)/src/config
SDKCONFIG_JS = $(SDKCONFIG_JS_DIR)/sdkconfig.js 
endif

include sources.mk
include targets.mk
include rules.mk
