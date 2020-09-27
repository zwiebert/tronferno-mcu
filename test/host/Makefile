# scans component directories for <component>/test/test_*.(c|cc|cpp) files and compiles them into test programs
# compiles any other portable component source files into build/obj.a
#
# note: esp-idf's on-chip-testing (unity-app) ignores .cc test files (it will work with .cpp)


.PHONY: all clean init tests build_lib sdkconfig

test_blacklist :=  test_config_keys

ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

make := make --no-print-dir -s
grep_results := ! grep -H -e IGNORE -e FAIL -e Aborted -e failed

THIS_ROOT := $(realpath .)
SRC_BASE :=$(THIS_ROOT)/../..
BUILD_BASE ?= $(THIS_ROOT)/build
BUILD_PATH=$(BUILD_BASE)/test.host
BUILD_OBJ_PATH=$(BUILD_PATH)/obj
BUILD_OBJ_LIB=$(BUILD_PATH)/obj.a
#LOG_BASE = $(realpath $(BUILD_PATH)/logs)
#TEST_BASE =$(realpath $(BUILD_PATH))
LOG_BASE= $(BUILD_PATH)/logs
TEST_BASE= $(BUILD_PATH)
TEST_ENV=$(TEST_BASE)/test_env

COMP_LIB_DIR=comp/library
COMP_APP_DIR=comp/app
COMP_EXT_COMPMCU_DIR=comp/external/components-mcu/components
COMP_DIRS = $(COMP_LIB_DIR) $(COMP_APP_DIR) $(COMP_EXT_COMPMCU_DIR)

COMP_NAMES = $(notdir $(shell find $(addprefix $(SRC_BASE)/,$(addsuffix /*,$(COMP_DIRS))) -maxdepth 0 -type d))
COMP_PATHS = $(shell find $(addprefix $(SRC_BASE)/,$(addsuffix /*,$(COMP_DIRS))) -maxdepth 0 -type d)
COMP_INC_PATHS = $(addsuffix /include,$(COMP_PATHS))
SRC_DIR_EXCLUDE = esp8266 esp32 nanopb node_modules
SRC_DIR_EXCLUDE_FIND = 
SDKCONFIG := $(SRC_BASE)/src/esp32/build/config/sdkconfig.h
HOST_SDKCONFIG := $(SRC_BASE)/$(COMP_APP_DIR)/app_config/include/app_config/host/sdkconfig.h

comm=$(addprefix $(BUILD_OBJ_PATH)/,test_runner/setup.o unity/src/unity.o test_runner/test_runner.o)
TEST_RUNNER_C_SRC = test_runner/setup.c unity/src/unity.c test_runner/test_runner.c test_runner/mock.cc
TEST_RUNNER_C_OBJ = $(addprefix $(BUILD_OBJ_PATH)/,$(addsuffix .o,$(TEST_RUNNER_C_SRC)))

CXX_SRC := $(shell find $(COMP_PATHS) '(' -name '*.cc' -or -name '*.cpp' ')' \
                   -and -not '(' -wholename '*/esp8266/*' -or  -wholename '*/esp32/*' -or -wholename '*/nanopb/*' \
                   -or -wholename '*/node_modules/*' ')' )
C_SRC := $(shell find $(COMP_PATHS) -name '*.c' \
                   -and -not '(' -wholename '*/esp8266/*' -or  -wholename '*/esp32/*' -or -wholename '*/nanopb/*' \
                   -or -wholename '*/jsmn/*test*' -or -wholename '*/node_modules/*' -or -wholename '*/esp8266/spiffs/*' ')')
TEST_SRC := $(shell find $(COMP_PATHS) -maxdepth 2 -and '(' -name '*.c' -or -name '*.cc' -or -name '*.cpp' ')' -and -wholename '*/test/test_*')
                   
C_OBJ         := $(addsuffix .o,$(subst $(SRC_BASE)/,$(BUILD_OBJ_PATH)/,$(C_SRC))) $(TEST_RUNNER_C_OBJ)
CXX_OBJ       := $(addsuffix .o,$(subst $(SRC_BASE)/,$(BUILD_OBJ_PATH)/,$(CXX_SRC)))
VALID_OBJ     = $(shell find $(BUILD_OBJ_PATH) -name '*.o' -and -not -wholename '*/test/test_*.o')
TEST_OBJ         := $(addsuffix .o,$(subst $(SRC_BASE)/,$(BUILD_OBJ_PATH)/,$(TEST_SRC)))
#TEST_OBJ      = $(shell find $(BUILD_OBJ_PATH) -name '*.o' -and -wholename '*/test/test_*.o')
TEST_NAMES    = $(filter-out $(test_blacklist),$(basename $(basename $(notdir $(TEST_OBJ)))))
TEST_PROGS    = $(addprefix $(TEST_BASE)/,$(TEST_NAMES))
TEST_LOGS     = $(addprefix $(LOG_BASE)/,$(addsuffix .log,$(TEST_NAMES)))


EXTRA_INCDIR = test_runner $(COMP_INC_PATHS)
INCLUDE := $(addprefix -I,$(EXTRA_INCDIR))

C_WARN += -Wall -Wextra -Wno-unused-result -Wno-missing-field-initializers -Wno-clobbered
TESTING_DEFS :=
CPPFLAGS :=  -MMD -DHOST_TESTING -DTEST_HOST -DTEST $(TESTING_DEFS) $(INCLUDE)
CFLAGS := -g3 $(C_WARN) -flto -O2
CXXFLAGS := -std=gnu++2a -g3 $(C_WARN) -fpermissive -flto -O2 -flto
LDLIBS += $(BUILD_OBJ_LIB) -lc


all:
	$(Q) $(make)  -s -j init C_WARN='-w'
	$(make)  -s $(TEST_NAMES) grep C_WARN='-w'

tests: init $(TEST_NAMES) grep

.PHONY: $(TEST_NAMES)
$(TEST_NAMES):
	$(Q) $(make) $(addsuffix .log,$(addprefix $(LOG_BASE)/,$@))

grep:
	$(Q) $(grep_results) $(LOG_BASE)/*.log

clean:
	$(Q) -rm -rf build/
	$(Q) -rm -rf test_env/*
	$(Q) -rm -rf logs/*

init: build_lib


CM_BUILD_PATH = $(BUILD_BASE)/cm

.PHONY: test_cm
test_cm:
	mkdir -p $(CM_BUILD_PATH)
	cmake -B $(CM_BUILD_PATH) -S $(THIS_ROOT)
	make -C $(CM_BUILD_PATH) all test

# create log file by running test
$(LOG_BASE)/%.log:$(TEST_BASE)/%
	$(Q) mkdir -p $(dir $@) $(TEST_ENV)/$(basename $(@F))
	$(Q) cd $(TEST_ENV)/$(basename $(@F)) && echo "** Run test: $<\n" && $< >$@ 2>&1 || ($(grep_results) $@)


# grep log file
%:$(LOG_BASE)/%.log
	$(vecho) "grep $@"
	$(Q) $(grep_results) $@
	
# make lib from all non-test objs
$(BUILD_OBJ_LIB): $(VALID_OBJ)
	  $(vecho) "AR $@"
		$(Q) $(AR) cr $@ $^

# create objs by compiling sources
$(BUILD_OBJ_PATH)/%.c.o : ./%.c
	$(vecho) "CC $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.c) -o $@ $<
$(BUILD_OBJ_PATH)/%.cc.o : ./%.cc
	$(vecho) "CXX $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.cc) -o $@ $<
$(BUILD_OBJ_PATH)/%.c.o : $(SRC_BASE)/%.c
	$(vecho) "CC $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.c) -o $@ $<  || (echo "$@" >> $(BUILD_PATH)/failed_compiles.txt && false)
$(BUILD_OBJ_PATH)/%.cc.o : $(SRC_BASE)/%.cc
	$(vecho) "CXX $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.cc) -o $@  $<  || (echo "$@" >> $(BUILD_PATH)/failed_compiles.txt && false)
$(BUILD_OBJ_PATH)/%.cpp.o : $(SRC_BASE)/%.cpp
	$(vecho) "CXX $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.cc) -o $@  $<  || (echo "$@" >> $(BUILD_PATH)/failed_compiles.txt && false)

print_test_names:
	$(info $(TEST_NAMES))
print_test_progs:
	$(info $(TEST_PROGS))
print_test_logs:
	$(info $(TEST_LOGS))
print_test_obj:
	$(info $(TEST_OBJ))
print_cxx_src:
	$(info $(CXX_SRC))
print_test_src:
	$(info $(TEST_SRC))
print_cxx_obj:
	$(info $(CXX_OBJ))
print_c_src:
	$(info $(C_SRC))
print_c_obj:
	$(info $(C_OBJ))
print_valid_obj:
	$(info $(VALID_OBJ))
print_comp_names:
	$(info $(COMP_NAMES))
print_comp_paths:
	$(info $(COMP_PATHS))
print_comp_inc_paths:
	$(info $(COMP_INC_PATHS))

build_lib:
	-make -j -k $(CXX_OBJ) $(C_OBJ)
	make $(BUILD_OBJ_LIB)

sdkconfig:
	make -B $(HOST_SDKCONFIG)

#test_runner/sdkconfig.h: $(SDKCONFIG)
#	grep CONFIG_TF_ $< >$@
$(HOST_SDKCONFIG): $(SDKCONFIG)
	grep CONFIG_TF_ $< >$@

################################

define build_test
$(BUILD_PATH)/$(basename $(basename $(notdir $(1)))): $(1) $(BUILD_OBJ_LIB)
	$(vecho) "LINK.cc $(1)"
	$(Q)  $(LINK.cc) $$^ $(LOADLIBES) $(LDLIBS) -o $$@
endef

$(foreach test_obj,$(TEST_OBJ),$(eval $(call build_test,$(test_obj))))

##################
DEPFILES = $(shell find $(BUILD_OBJ_PATH) -name '*.d')
include $(DEPFILES)



################ New Cmake based test build ########################
CM_BUILD_PATH=$(BUILD_BASE)/cm.test.host

.PHONY: test.cm.config test.cm.build

test.cm.config:
	rm -r $(CM_BUILD_PATH)
	mkdir -p $(CM_BUILD_PATH)
	cmake -B $(CM_BUILD_PATH)

	
test.cm.build:
	$(make) -k -j -C $(CM_BUILD_PATH) VERBOSE=1

test.cm.ctest:
	$(make)  -C $(CM_BUILD_PATH) VERBOSE=1  test
	
	
