# scans component directories for <component>/test/test_*.(cc|c) files and compiles them into test programs
# compiles any other portable component source files into build/obj.a


.PHONY: all clean init tests build_lib

ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

make := make --no-print-dir -s
grep_results := ! grep -H -e IGNORE -e FAIL -e Aborted

SRC_BASE=../..
BUILD_BASE=./build
BUILD_OBJ_PATH=$(BUILD_BASE)/obj
LOG_BASE=./logs
TEST_BASE=./build
COMP_DIR=src/components
COMPSHARE_DIR=components-mcu/components
COMP_DIRS = $(COMP_DIR) $(COMPSHARE_DIR)
COMP_NAMES = $(notdir $(shell find $(addprefix $(SRC_BASE)/,$(addsuffix /*,$(COMP_DIRS))) -maxdepth 0 -type d))
COMP_PATHS = $(shell find $(addprefix $(SRC_BASE)/,$(addsuffix /*,$(COMP_DIRS))) -maxdepth 0 -type d)
COMP_INC_PATHS = $(addsuffix /include,$(COMP_PATHS))
SRC_DIR_EXCLUDE = esp8266 esp32 nonopb node_modules
SRC_DIR_EXCLUDE_FIND = 

comm=$(addprefix $(BUILD_OBJ_PATH)/,test_runner/setup.o unity/src/unity.o test_runner/test_runner.o)
TEST_RUNNER_C_SRC = test_runner/setup.c unity/src/unity.c test_runner/test_runner.c
TEST_RUNNER_C_OBJ = $(addprefix $(BUILD_OBJ_PATH)/,$(addsuffix .o,$(TEST_RUNNER_C_SRC)))

CXX_SRC := $(shell find $(COMP_PATHS) '(' -name '*.cc' -or -name '*.cpp' ')' \
                   -and -not '(' -wholename '*/esp8266/*' -or  -wholename '*/esp32/*' -or -wholename '*/nanopb/*' \
                   -or -wholename '*/node_modules/*' ')' )
C_SRC := $(shell find $(COMP_PATHS) -name '*.c' \
                   -and -not '(' -wholename '*/esp8266/*' -or  -wholename '*/esp32/*' -or -wholename '*/nanopb/*' \
                   -or -wholename '*/jsmn/*' -or -wholename '*/node_modules/*' ')') 
                   
C_OBJ         := $(addsuffix .o,$(subst $(SRC_BASE)/,$(BUILD_OBJ_PATH)/,$(C_SRC))) $(TEST_RUNNER_C_OBJ)
CXX_OBJ       := $(addsuffix .o,$(subst $(SRC_BASE)/,$(BUILD_OBJ_PATH)/,$(CXX_SRC)))
VALID_OBJ     = $(shell find $(BUILD_OBJ_PATH) -name '*.o' -and -not -wholename '*/test/test_*.o')
TEST_OBJ      = $(shell find $(BUILD_OBJ_PATH) -name '*.o' -and -wholename '*/test/test_*.o')
TEST_NAMES    = $(basename $(basename $(notdir $(TEST_OBJ))))
TEST_PROGS    = $(addprefix $(TEST_BASE)/,$(TEST_NAMES))
TEST_LOGS     = $(addprefix $(LOG_BASE)/,$(addsuffix .log,$(TEST_NAMES)))


EXTRA_INCDIR = test_runner $(COMP_INC_PATHS)
INCLUDE := $(addprefix -I,$(EXTRA_INCDIR))

C_WARN += -Wall -Wextra -Wno-unused-result -Wno-missing-field-initializers -Wno-clobbered
TESTING_DEFS :=
CPPFLAGS :=  -MMD -DHOST_TESTING -DTEST_HOST -DTEST $(TESTING_DEFS) $(INCLUDE)
CFLAGS := -g3 $(C_WARN) -flto -O1
CXXFLAGS := -std=gnu++2a -g3 $(C_WARN) -fpermissive -flto -O1
LDLIBS += build/obj.a -lc


all:
	$(Q) $(make)  -s -j init C_WARN='-w' &&	$(make)  -s -j $(TEST_NAMES) grep C_WARN='-w'

tests: init $(TEST_NAMES) grep

.PHONY: $(TEST_NAMES)
$(TEST_NAMES):
	$(Q) $(make) $(addsuffix .log,$(addprefix $(LOG_BASE)/,$@))

grep:
	$(Q) $(grep_results) $(LOG_BASE)/*.log

clean:
	$(Q) -rm -rf build/
	$(Q) -rm -rf test_env/*

init: build_lib


# create log file by running test
$(LOG_BASE)/%.log:$(TEST_BASE)/%
	$(Q) mkdir -p $(dir $@) test_env/$(basename $(@F))
	$(Q) cd ./test_env/$(basename $(@F)) && echo "** Run test: $<\n" && ../../$< >../../$@ 2>&1 || ($(grep_results) ../../$@)


# grep log file
%:$(LOG_BASE)/%.log
	$(vecho) "grep $@"
	$(Q) $(grep_results) $@
	
# make lib from all non-test objs
$(BUILD_BASE)/obj.a: $(VALID_OBJ)
		$(Q) $(AR) cr $@ $^

# create objs by compiling sources
$(BUILD_OBJ_PATH)/%.c.o : ./%.c
	$(vecho) "CC $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.c) -o $@ $<
$(BUILD_OBJ_PATH)/%.c.o : $(SRC_BASE)/%.c
	$(vecho) "CC $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.c) -o $@ $<
$(BUILD_OBJ_PATH)/%.cc.o : $(SRC_BASE)/%.cc
	$(vecho) "CXX $@"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(COMPILE.cc) -o $@  $<

print_test_names:
	$(info $(TEST_NAMES))
print_test_progs:
	$(info $(TEST_PROGS))
print_test_obj:
	$(info $(TEST_OBJ))
print_cxx_src:
	$(info $(CXX_SRC))
print_cxx_obj:
	$(info $(CXX_OBJ))
print_c_src:
	$(info $(C_SRC))
print_c_obj:
	$(info $(C_OBJ))
print_comp_names:
	$(info $(COMP_NAMES))
print_comp_paths:
	$(info $(COMP_PATHS))
print_comp_inc_paths:
	$(info $(COMP_INC_PATHS))

build_lib:
	-make -j -k $(CXX_OBJ) $(C_OBJ)
	make build/obj.a

################################

define build_test
build/$(basename $(basename $(notdir $(1)))): $(1)
	$(vecho) "LINK.cc $(1)"
	$(Q)  $(LINK.cc) $$< $(LOADLIBES) $(LDLIBS) -o $$@
endef

$(foreach test_obj,$(TEST_OBJ),$(eval $(call build_test,$(test_obj))))

##################
DEPFILES = $(shell find $(BUILD_OBJ_PATH) -name '*.d')
include $(DEPFILES)
