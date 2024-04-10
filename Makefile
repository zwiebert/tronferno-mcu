.PHONY: clean all test rebuild http_data print-help

flavors = esp32 esp32dbg esp32wlan esp32lan esp32test

flavor ?= esp32

default: help

clean : esp32-test-clean esp32-fullclean http_clean
	make -C test/esp32 clean


help:
	@less docs/make_help.txt

#PROXY (env: MCU_IP_ADDR, PROXY_TCP_PORT)
#==========================================
.PHONY: http_proxy http_clean
http_proxy:
	cd comp/app/app_webapp && make BUILD_DIR=$(BUILD_BASE)/app_webapp proxy
http_clean:
	cd comp/app/app_webapp && make BUILD_DIR=$(BUILD_BASE)/app_webapp clean


####### ESP32 build command ############
PORT ?=/dev/serial/by-path/pci-0000:03:00.0-usb-0:3.1:1.0-port0

V ?= 0

ifneq "$(V)" "0"
esp32_build_opts += -v
endif

# Add the python binary of python-venv to the path to make idf.py work in Eclipse
# XXX: maybe its better to do this from the shell script which starts Eclipse (which runs export.sh anyway)
export PATH := $(IDF_PYTHON_ENV_PATH)/bin:$(PATH) 

env:
	env | grep IDF

THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build/$(flavor)
esp32_build_dir := $(BUILD_BASE)
esp32_src_dir := $(THIS_ROOT)/src/$(flavor)
tmp_build_dir := /tmp/tronferno-mcu/build

esp32_cmake_generator := -G Ninja

esp32_build_args := $(esp32_cmake_generator) -C $(esp32_src_dir) -B $(esp32_build_dir)  -p $(PORT)  $(esp32_build_opts)
esp32_build_cmd := idf.py $(esp32_build_args)
esp32_cmake_cmd := /usr/bin/cmake -S $(esp32_src_dir) -B $(esp32_build_dir) $(esp32_cmake_generator)


######### ESP32 Targets ##################
esp32_tgts_auto := menuconfig clean fullclean app flash monitor gdb gdbgui gdbtui reconfigure

.PHONY: esp32-all-force esp32-rebuild
.PHONY: esp32-all esp32-flash esp32-flash-ocd
.PHONY: esp32-dot
.PHONY: FORCE

define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1):
	$(esp32_build_cmd) $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))




#need bash for "source" command
SHELL := /bin/bash
#provide IDF_PATH if not in env
export IDF_PATH ?= "$(HOME)/esp/esp-idf"
.PHONY: idf_make
#Rule for use in vscode, which does not inherit the idf exports
#so we source $IDF_PAtH/export.sh for each run of make
#
#usage: make idf_target my_target=<target>
#example: make idf_target my_target=esp32-all  (instead of make esp32-all)
#
idf_make:
	(source "$(IDF_PATH)/export.sh" && make $(my_target))


esp32-all:
	$(esp32_build_cmd) reconfigure all


############ Graphviz ######################
gv_build_dir := $(tmp_build_dir)
gv_dot_file := $(gv_build_dir)/tfmcu.dot
gv_png_file :=  $(gv_build_dir)/tfmcu.png


esp32-png: $(gv_png_file)
esp32-dot: $(gv_dot_file)

esp32-png-view: $(gv_png_file)
	xdg-open $(gv_png_file)

$(gv_dot_file): FORCE $(gv_build_dir)
	$(esp32_cmake_cmd) --graphviz=$(gv_dot_file)
	
%.png:%.dot
	dot -Tpng -o $@ $<

$(gv_build_dir):
	mkdir -p $@
	
.PHONY: FORCE
############ openocd ###################################
esp32_ocd_sh :=  "$(esp32_src_dir)/esp32_ocd.sh" $(esp32_src_dir) $(esp32_build_dir)

esp32-flash-ocd:
	$(esp32_ocd_sh) flash
esp32-flash-app-ocd:
	$(esp32_ocd_sh) flash_app
esp32-ocd:
	$(esp32_ocd_sh)  server
esp32-ocd-loop:
	$(esp32_ocd_sh) server_loop

########### Unit Testing ###############
esp32_test_tgts_auto := build clean flash run all all-ocd flash-ocd flash-app-ocd

define GEN_RULE
.PHONY: esp32-$(1)
esp32-test-$(1):
	make -C test/esp32 $(1)  port=$(PORT)
endef
$(foreach tgt,$(esp32_test_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))


############## On Host ########################
HOST_TEST_BUILD_PATH=$(BUILD_BASE)/../host/test
HOST_TEST_SRC_PATH=$(THIS_ROOT)/src/host_test

.PHONY: test.cm.configure test.cm.build  test.cm.kconfgen

# let menuconfig generate config/sdkconfig.(cmake|h)
# disregard the rest of build/hos_test
# we still keep some minimal sdkconfig.(cmake|h) for automated testing where we don't have esp-idf
test.cm.menuconfig:
	make flavor=host_test esp32-menuconfig esp32-reconfigure

config_h:=$(THIS_ROOT)/src/host_test/config/sdkconfig.h
config_cmake:=$(THIS_ROOT)/src/host_test/config/sdkconfig.cmake
_config:=$(THIS_ROOT)/src/host_test/sdkconfig


test.cm.kconfgen $(config_h) $(config_cmake): $(_config)
	python -m kconfgen  --kconfig  $(THIS_ROOT)/comp/config/app_config/Kconfig.projbuild --config $(_config) --output header $(config_h) --output cmake $(config_cmake)
test.cm.copy_config:
	cp $(THIS_ROOT)/build/host_test/config/sdkconfig.h $(THIS_ROOT)/build/host_test/config/sdkconfig.cmake $(THIS_ROOT)/src/host_test/config/


test.cm.configure:
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)/config
	(cp $(THIS_ROOT)/build/host_test/config/sdkconfig.h $(THIS_ROOT)/build/host_test/config/sdkconfig.cmake $(HOST_TEST_BUILD_PATH)/config/ || \
	cp $(THIS_ROOT)/src/host_test/config/sdkconfig.h $(THIS_ROOT)/src/host_test/config/sdkconfig.cmake $(HOST_TEST_BUILD_PATH)/config/)

	cmake -B $(HOST_TEST_BUILD_PATH) -D BUILD_HOST_TESTS=ON -S  $(HOST_TEST_SRC_PATH) #-G Ninja)

cm_build := make -C $(HOST_TEST_BUILD_PATH) -k -j  -s --no-print-dir $(make_verbose_opts)
#cm_build := (cd $(HOST_TEST_BUILD_PATH) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --verbose $(ninja_verbose_opts))

	
test.cm.build:
	$(cm_build)

test.cm.ctest: test.cm.build
	cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure
	
TEST ?= test.weather.test_
test.cm.ctest.regex: test.cm.build
	(cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure -R "$(TEST)")


host-test-all:
	make -s --no-print-directory  test.cm.configure test.cm.ctest



############# Doxygen ###################
doxy_flavors=usr dev api
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy
DOXYFILE_PATH=$(THIS_ROOT)
include doxygen_rules.mk


$(DOXY_BUILD_PATH)/dev/input_files: $(DOXY_BUILD_PATH)/dev FORCE
	git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | sed "s~^~$(THIS_ROOT)/~" > $@
	cd comp/components-mcu && git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | sed "s~^~$(THIS_ROOT)/comp/components-mcu/~" >> $@
$(DOXY_BUILD_PATH)/api/input_files: $(DOXY_BUILD_PATH)/api FORCE
	git ls-files '*.h' '*.hh' | fgrep include | sed "s~^~$(THIS_ROOT)/~" > $@
	cd comp/components-mcu && git ls-files '*.h' '*.hh' | fgrep include | sed "s~^~$(THIS_ROOT)/comp/components-mcu/~" >> $@

$(DOXY_BUILD_PATH)/usr/input_files: $(DOXY_BUILD_PATH)/usr FORCE
	echo "" > $@
	
FORCE:

############# CLI Terminal ##############
MCU_IP_ADDR ?= 192.168.1.69

telnet:
	telnet $(MCU_IP_ADDR) 7777


