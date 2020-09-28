# scans component directories for <component>/test/test_*.(c|cc|cpp) files and compiles them into test programs
# compiles any other portable component source files into build/obj.a
#
# note: esp-idf's on-chip-testing (unity-app) ignores .cc test files (it will work with .cpp)


.PHONY: all clean init tests build_lib sdkconfig


all:
	make -j 1 test.cm.configure test.cm.build test.cm.ctest

test_blacklist :=  test_config_keys

ifeq ("$(V)","1")
Q :=
vecho := @true
ninja_verbose_opts := -v
make_verbose_opts := VERBOSE=1
else
Q := @
vecho := @echo

endif

make := make --no-print-dir -s
grep_results := ! grep -H -e IGNORE -e FAIL -e Aborted -e failed

THIS_ROOT := $(realpath .)
SRC_BASE :=$(THIS_ROOT)/../..
BUILD_BASE ?= $(THIS_ROOT)/build

################ New Cmake based test build ########################
CM_BUILD_PATH=$(BUILD_BASE)/cm.test.host

.PHONY: test.cm.configure test.cm.build

test.cm.configure:
	rm -fr $(CM_BUILD_PATH)
	mkdir -p $(CM_BUILD_PATH)
	cmake -B $(CM_BUILD_PATH) -G Ninja

cm_build := $(make) C $(CM_BUILD_PATH)  -k -j  $(make_verbose_opts)
cm_build := ninja -C $(CM_BUILD_PATH)   -k 0 $(ninja_verbose_opts) 
	
test.cm.build:
	$(cm_build)

test.cm.ctest:
	$(cm_build) test
	
	
