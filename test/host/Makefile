.PHONY: all tests clean_out init all_tests


TESTS =  test_pos test_config  test_timer_state test_alias test_timer_data test_msg_tx test_prg test_kvs test_gm 

.PHONY: $(TESTS)

make = make --no-print-dir


ifeq ($(V),)
Q:=@
endif

grep_results := grep -H -e IGNORE -e FAIL -e Aborted 

all_tests:
	$(Q) $(make) all C_WARN='-w'

all: clean_out init
	@echo "\n### Making and running tests: " $(TESTS) "\n"
	$(Q) $(make) -B tests
	
$(TESTS):
	$(Q) $(make) -B $(addsuffix .log,$(addprefix build/,$@))

clean_out:
	$(Q) -rm -f build/test_*.out
	$(Q) -rm -f test_env/*
init:
	$(Q) mkdir -p build
	$(Q) mkdir -p test_env

	
tests = $(addsuffix .log,$(addprefix build/,$(TESTS)))
tests: $(tests)
	$(Q) $(grep_results) build/*.log  || true

SRC_BASE=../..
COMP_DIR=src/components
COMPSHARE_DIR=components-mcu/components
COMPSHARE_PATH := $(SRC_BASE)/$(COMPSHARE_DIR)
COMP_PATH := $(SRC_BASE)/$(COMP_DIR)
EXTRA_INCDIR = test_runner #$(COPMSHARE_PATH)/storage/spiffs/src
C_WARN ?= -Wall -Wextra -Wno-unused-result -Wno-missing-field-initializers


COMPSHARE = txtio net debug cli key_value_store misc misc_time config_kvs userio net_mqtt net_http_server storage storage/esp8266/spiffs/src
COMPONENTS = gpio config cli_app fernotron  fernotron_txtio  fernotron_sep fernotron_cuas fernotron_alias fernotron_pos fernotron_auto userio_app app app_config net_mqtt_app 

define GEN_COMPONENTS
EXTRA_INCDIR += $(SRC_BASE)/$(1)/$(2)/include #$(SRC_BASE)/$(1)/$(2)/include/$(2)
MODULES += $(1)/$(2)
ifneq ("$(wildcard $(SRC_BASE)/$(1)/$(2)/esp8266)","")
MODULES += $(1)/$(2)/esp8266
endif
endef
$(foreach component,$(COMPSHARE),$(eval $(call GEN_COMPONENTS,$(COMPSHARE_DIR),$(component))))
$(foreach component,$(COMPONENTS),$(eval $(call GEN_COMPONENTS,$(COMP_DIR),$(component))))

INCLUDE := $(addprefix -I,$(EXTRA_INCDIR))

#build/test_%.out:
	#$(Q) g++  -std=gnu++2a -g3 $(C_WARN) -fpermissive -flto -O1 -o $@  $^ unity/src/unity.c test_runner/test_runner.c $(setup) $(INCLUDE) -DHOST_TESTING -DTEST_HOST -DTEST -lm #-H

CPPFLAGS := -DHOST_TESTING -DTEST_HOST -DTEST $(INCLUDE)
CFLAGS := -g3 $(C_WARN) -flto -O1 
CC := gcc
CXXFLAGS := -std=gnu++2a -g3 $(C_WARN) $(CPP_FLAGS) -fpermissive -flto -O1
CXX := g++


%.log:%.out
	$(Q) mkdir -p test_env
	$(Q) cd ./test_env && echo "** Run test: $<\n" && ../$< >../$@ 2>&1 || ($(grep_results) ../$@ && false)
	$(Q) $(grep_results) $@ || true

./build/config_kvs.o: $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.o)
	$(CC) -c -o $@ $< 
	
%.c.o : %.c
	$(CC)  $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
%.cc.o : %.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@  $<


.PHONY:cfiles

cfiles: build/config_kvs.o

comm=test_runner/setup.o unity/src/unity.o test_runner/test_runner.o
################################
build/test_pos.out:   $(comm) $(addprefix $(COMP_PATH)/fernotron_pos/,test/test_positions.cc.o shutter_pct.cc.o shutter_prefs_kvs.cc.o move.cc.o move_buf.cc.o) $(addprefix $(COMP_PATH)/fernotron/,gmbitmask_kvs.cc.o)  $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o)
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_config.out: $(comm) $(addprefix $(COMP_PATH)/config/,test/test_config.cc.o config.cc.o config_kvs.cc.o config_stor.cc.o) $(addprefix $(COMP_PATH)/fernotron/,gmbitmask_kvs.cc.o) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o storage/esp8266/storage.c.o misc/esp32/itoa.c.o config_kvs/config_kvs.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_kvs.out:  $(comm) $(addprefix $(COMPSHARE_PATH)/key_value_store/,test/test_kvs.o)  $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_gm.out:  $(comm)  $(addprefix $(COMP_PATH)/fernotron/,test/test_gm.cc.o gmbitmask_kvs.cc.o) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_prg.out:  $(comm) $(addprefix $(COMP_PATH)/fernotron/,test/test_prg.cc.o astro.cc.o fer_rawmsg_build.cc.o) $(COMPSHARE_PATH)/misc/bcd.c.o $(COMPSHARE_PATH)/misc/sun.c.o 
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_msg_tx.out:  $(comm) $(addprefix $(COMP_PATH)/fernotron/,test/test_msg_tx.cc.o fer_msg_tx.cc.o fer_msg_tx_queue.cc.o fer_radio_rx.cc.o fer_radio_tx.cc.o fer_radio_parity.cc.o fer_msg_basic.cc.o fer_rawmsg_buffer.cc.o fer_rawmsg_build.cc.o astro.cc.o) $(addprefix $(COMPSHARE_PATH)/, misc/bcd.c.o misc/sun.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_timer_data.out:  $(comm)  $(addprefix $(COMP_PATH)/fernotron_auto/,test/test_timer_data.cc.o timer_data.cc.o timer_data_kvs.cc.o)  $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_timer_state.out:  $(comm) $(addprefix $(COMP_PATH)/fernotron_auto/,test/test_timer_state.cc.o timer_minutes.cc.o timer_event.cc.o timer_data.cc.o timer_data_kvs.cc.o) $(COMP_PATH)/fernotron/astro.cc.o $(addprefix $(COMP_PATH)/fernotron/,gmbitmask_kvs.cc.o) $(addprefix $(COMPSHARE_PATH)/,storage/esp8266/storage.c.o misc/bcd.c.o misc/sun.c.o key_value_store/esp8266/kvs_posix.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^
build/test_alias.out:  $(comm) $(addprefix $(COMP_PATH)/fernotron_alias/,test/test_alias.cc.o pairings_kvs.cc.o) $(addprefix $(COMP_PATH)/fernotron/,gmbitmask_kvs.cc.o) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o misc/esp32/itoa.c.o )
	$(CXX) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) -o $@  $^


