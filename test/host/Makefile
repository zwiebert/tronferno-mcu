.PHONY: all tests clean_out init

TESTS =  test_config  test_timer_state test_alias test_timer_data test_msg_tx test_prg test_kvs test_gm 

.PHONY: $(TESTS)

make = make --no-print-dir



Q:=@
grep_results := grep -H -e IGNORE -e FAIL -e Aborted 

all: clean_out init
	@echo "\n### Making and running tests: " $(TESTS) "\n"
	$(Q) $(make) -B tests
	
$(TESTS):
	$(make) -B $(addsuffix .log,$(addprefix build/,$@))

clean_out:
	-rm -f build/test_*.out
	-rm -f test_env/*
init:
	mkdir -p build
	mkdir -p test_env

	
tests = $(addsuffix .log,$(addprefix build/,$(TESTS)))
tests: $(tests)
	$(Q) $(grep_results) build/*.log  || true

SRC_BASE=../..
COMP_DIR=src/components
ifneq (,$(wildcard $(SRC_BASE)/local))
COMPSHARE_DIR=local/components-mcu/components
else
COMPSHARE_DIR=components-mcu/components
endif
COMPSHARE_PATH := $(SRC_BASE)/$(COMPSHARE_DIR)
COMP_PATH := $(SRC_BASE)/$(COMP_DIR)
EXTRA_INCDIR = test_runner #$(COPMSHARE_PATH)/storage/spiffs/src

COMPSHARE = txtio net debug cli key_value_store misc misc_time userio net_mqtt net_http_server storage storage/spiffs/src
COMPONENTS = gpio config cli_app fernotron  fernotron_txtio  fernotron_sep fernotron_cuas fernotron_alias fernotron_pos fernotron_auto userio_app app app_config net_mqtt_app 

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

build/test_%.out:
	$(Q) gcc -g3 -Wall -o $@  $^ unity/src/unity.c test_runner/test_runner.c $(setup) $(INCLUDE) -DHOST_TESTING -DTEST_HOST -DTEST -lm


%.log:%.out
	$(Q) mkdir -p test_env
	$(Q) cd ./test_env && echo "** Run test: $<\n" && ../$< >../$@ 2>&1 || ($(grep_results) ../$@ && false)
	$(Q) $(grep_results) $@ || true


setup=test_runner/setup.c
################################
build/test_config.out:   $(addprefix $(COMP_PATH)/config/,test/test_config.c config.c config_kvs.c config_stor.c esp8266/config.c) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c storage/esp8266/storage.c misc/esp32/itoa.c)
build/test_kvs.out:  $(addprefix $(COMPSHARE_PATH)/key_value_store/,test/test_kvs.c esp8266/kvs_posix.c)
build/test_gm.out:   $(addprefix $(COMP_PATH)/fernotron/,test/test_gm.c gmbitmask_kvs.c) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c)
build/test_prg.out:  $(addprefix $(COMP_PATH)/fernotron/,test/test_prg.c astro.c fer_rawmsg_build.c) $(COMPSHARE_PATH)/misc/bcd.c $(COMPSHARE_PATH)/misc/sun.c
build/test_msg_tx.out:  $(addprefix $(COMP_PATH)/fernotron/,test/test_msg_tx.c fer_msg_tx.c fer_msg_basic.c fer_rawmsg_buffer.c fer_rawmsg_build.c astro.c) $(addprefix $(COMPSHARE_PATH)/, misc/bcd.c misc/sun.c)
build/test_timer_data.out:   $(addprefix $(COMP_PATH)/fernotron_auto/,test/test_timer_data.c timer_data.c timer_data_fs.c)  $(COMPSHARE_PATH)/storage/esp32/storage.c
build/test_timer_state.out:  $(addprefix $(COMP_PATH)/fernotron_auto/,test/test_timer_state.c timer_state.c timer_data.c timer_data_fs.c) $(COMP_PATH)/fernotron/astro.c $(addprefix $(COMPSHARE_PATH)/, storage/esp8266/storage.c misc/bcd.c misc/sun.c)
build/test_alias.out:  $(addprefix $(COMP_PATH)/fernotron_alias/,test/test_alias.c pairings_kvs.c) $(addprefix $(COMP_PATH)/fernotron/,gmbitmask_kvs.c) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c misc/esp32/itoa.c)


