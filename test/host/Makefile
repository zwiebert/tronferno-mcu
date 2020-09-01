.PHONY: all tests clean_out clean init all_tests

ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

make := make --no-print-dir
grep_results := ! grep -H -e IGNORE -e FAIL -e Aborted




SRC_BASE=../..
BUILD_BASE=./build/src
COMP_DIR=src/components
COMPSHARE_DIR=components-mcu/components
COMPSHARE_PATH := $(BUILD_BASE)/$(COMPSHARE_DIR)
COMP_PATH := $(BUILD_BASE)/$(COMP_DIR)
COMPSHARE = txtio net debug cli key_value_store misc misc_time config_kvs userio net_mqtt net_http_server storage storage/esp8266/spiffs/src stm32 jsmn
COMPONENTS = gpio config cli_app fernotron  fernotron_txtio  fernotron_sep fernotron_cuas fernotron_alias fernotron_pos fernotron_auto userio_app app app_config net_mqtt_app http_server_content
comm=$(addprefix $(BUILD_BASE)/,test_runner/setup.o unity/src/unity.o test_runner/test_runner.o) $(addprefix $(COMPSHARE_PATH)/,key_value_store/esp8266/kvs_posix.c.o)
CXX_SRC := $(shell find $(SRC_BASE)/$(COMP_DIR) $(SRC_BASE)/$(COMPSHARE_DIR) -name '*.cc' -and -not '(' -wholename '*/esp8266/*' -or  -wholename '*/esp32/*' -or -wholename '*/nanopb/*' ')' )
C_SRC := $(shell find $(SRC_BASE)/$(COMP_DIR)  $(SRC_BASE)/$(COMPSHARE_DIR) -name '*.c' -and -not '(' -wholename '*/esp8266/*' -or  -wholename '*/esp32/*' -or -wholename '*/nanopb/*' -or -wholename '*/jsmn/*' ')')
C_OBJ         := $(patsubst %.c,$(BUILD_BASE)/%.c.o,$(subst $(SRC_BASE)/,,$(C_SRC)))
CXX_OBJ       := $(patsubst %.cc,$(BUILD_BASE)/%.cc.o,$(subst $(SRC_BASE)/,,$(CXX_SRC)))
VALID_OBJ     = $(shell find $(BUILD_BASE) -name '*.o' -and -not -wholename '*/test/test_*.o')  $(comm)
TEST_OBJ      = $(shell find $(BUILD_BASE) -name '*.o' -and -wholename '*/test/test_*.o')
TEST_NAMES    = $(basename $(basename $(notdir $(TEST_OBJ))))
TEST_PROGS    = $(addprefix build/,$(TEST_NAMES))
TEST_LOGS     = $(addsuffix .log,$(TEST_PROGS))


EXTRA_INCDIR = test_runner #$(COPMSHARE_PATH)/storage/spiffs/src
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

CC := gcc
CXX := g++
C_WARN ?= -Wall -Wextra -Wno-unused-result -Wno-missing-field-initializers
TESTING_DEFS :=
CPPFLAGS := -DHOST_TESTING -DTEST_HOST -DTEST $(TESTING_DEFS) $(INCLUDE)
CFLAGS := -g3 $(C_WARN) -flto -O1
CXXFLAGS := -std=gnu++2a -g3 $(C_WARN) -fpermissive -flto -O1
LDLIBS += build/obj.a -lc



%.log:%
	$(Q) mkdir -p test_env/$(basename $(@F))
	$(Q) cd ./test_env/$(basename $(@F)) && echo "** Run test: $<\n" && ../../$< >../../$@ 2>&1 || ($(grep_results) ../../$@)
	$(Q) $(grep_results) $@


$(BUILD_BASE)/%.o : ./%.c
	$(vecho) "CC $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC)  $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
$(BUILD_BASE)/%.c.o : $(SRC_BASE)/%.c
	$(vecho) "CC $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC)  $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
$(BUILD_BASE)/%.cc.o : $(SRC_BASE)/%.cc
	$(vecho) "CXX $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@  $<


all_tests:
	$(Q) $(make) all C_WARN='-w'

all: clean_out init
	@echo "\n### Making and running tests: " $(TESTS) "\n"
	$(Q) $(make) -B tests

tests: $(TEST_LOGS)
	$(Q) $(grep_results) build/*.log

.PHONY: $(TEST_NAMES)
$(TEST_NAMES):
	$(Q) $(make) -B $(addsuffix .log,$(addprefix build/,$@))

clean:
	$(Q) -rm -rf build/
	$(Q) -rm -rf test_env/*
clean_out:
	$(Q) -rm -f build/test_*
	$(Q) -rm -rf test_env/*
init: build_lib
	$(Q) mkdir -p build
	$(Q) mkdir -p test_env


print_test_names:
	$(Q) echo $(TEST_NAMES)
print_test_progs:
	$(Q) echo $(TEST_PROGS)
print_test_obj:
	$(Q) echo $(TEST_OBJ)
print_cxx_src:
	$(Q) echo $(CXX_SRC)


build/obj.a: $(VALID_OBJ)
		$(Q) $(AR) cr $@ $^


.PHONY: build_lib

build_lib:
	-make -j -k $(CXX_OBJ) $(C_OBJ)
	make build/obj.a

################################

define build_test
build/$(basename $(basename $(notdir $(1)))): $(1)
	$(vecho) "LD $(1) ($$@,$$<)"
	$(Q) $(CXX)  $(LDFLAGS) $$^ $(LDLIBS) -o $$@
endef

$(foreach test_obj,$(TEST_OBJ),$(eval $(call build_test,$(test_obj))))
