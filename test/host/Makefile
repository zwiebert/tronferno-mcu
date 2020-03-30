.PHONY: all tests

TESTS = test_kvs
tests = $(addprefix build/,$(TESTS))

all:
	@echo "\n### Making and running tests: " $(TESTS) "\n"
	@make -B tests

tests: $(tests)

print:
	echo $(tests)


unity_inc: $(realpath ./unity/src)

SRC_BASE=../..
COMP_DIR=src/components
ifneq (,$(wildcard $(SRC_BASE)/local))
COMPSHARE_DIR=local/components-mcu/components
else
COMPSHARE_DIR=components-mcu/components
endif
COMPSHARE_PATH := $(SRC_BASE)/$(COMPSHARE_DIR)
COMP_PATH := $(SRC_BASE)/$(COMP_DIR)

COMPSHARE = txtio net debug cli key_value_store misc misc_time userio net_mqtt net_http_server storage storage/spiffs/src
COMPONENTS = gpio config cli_app fernotron  fernotron_txtio  fernotron_sep fernotron_cuas fernotron_alias fernotron_pos fernotron_auto userio_app app app_config net_mqtt_app 
EXTRA_INCDIR = test_runner #$(COPMSHARE_PATH)/storage/spiffs/src


define GEN_COMPONENTS
EXTRA_INCDIR += $(SRC_BASE)/$(1)/$(2)/include $(SRC_BASE)/$(1)/$(2)/include/$(2)
MODULES += $(1)/$(2)
ifneq ("$(wildcard $(SRC_BASE)/$(1)/$(2)/esp8266)","")
MODULES += $(1)/$(2)/esp8266
endif
endef

$(foreach component,$(COMPSHARE),$(eval $(call GEN_COMPONENTS,$(COMPSHARE_DIR),$(component))))
$(foreach component,$(COMPONENTS),$(eval $(call GEN_COMPONENTS,$(COMP_DIR),$(component))))

INCLUDE := $(addprefix -I,$(EXTRA_INCDIR))

build/test_%:
	@gcc -g3 -o $@   test_runner/weak.c $^ unity/src/unity.c test_runner/test_runner.c  $(INCLUDE) -DHOST_TESTING
	@echo "## Run test: $@\n" && $@



################################
build/test_kvs: $(COMPSHARE_PATH)/key_value_store/test/test_kvs.c $(COMPSHARE_PATH)/key_value_store/esp8266/kvs_posix.c