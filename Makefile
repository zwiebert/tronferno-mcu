.PHONY: clean all test rebuild http_data print-help

flavors = esp32-release esp32-debug host-test

flavor ?= esp32-release

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

CMAKE_SRC_PATH :=$(THIS_ROOT)/configs/$(flavor)
BUILD_BASE ?=$(THIS_ROOT)/build/$(flavor)
BUILD_PATH :=$(BUILD_BASE)

tmp_build_dir :=/tmp/tronferno-mcu/build
compile_commands_json_latest :=$(THIS_ROOT)/build/compile_commands.json

esp32_cmake_generator := -G Ninja
esp32_build_args :=$(esp32_cmake_generator) -C $(CMAKE_SRC_PATH) -B $(BUILD_PATH)  -p $(PORT)  $(esp32_build_opts)
esp32_build_cmd :=idf.py $(esp32_build_args)
esp32_cmake_cmd :=/usr/bin/cmake -S $(CMAKE_SRC_PATH) -B $(BUILD_PATH) $(esp32_cmake_generator)


$(compile_commands_json_latest): FORCE
	cp $(BUILD_BASE)/compile_commands.json $(compile_commands_json_latest)

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
SHELL :=/bin/bash
#provide IDF_PATH if not in env
export IDF_PATH ?="$(HOME)/esp/esp-idf"
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
	make $(compile_commands_json_latest)


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
esp32_ocd_sh :=  "$(CMAKE_SRC_PATH)/esp32_ocd.sh" $(CMAKE_SRC_PATH) $(BUILD_PATH)

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


############## On Host Tests ##############
kconfigs=comp/config/app_config/Kconfig.projbuild external/*/Kconfig
TEST ?= test.weather.test_

include ./host_test_rules.mk


############# Doxygen ###################
doxy_flavors=usr dev api
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy
DOXYFILE_PATH=$(THIS_ROOT)
include doxygen_rules.mk


ext=external/*

	
FORCE:

############# CLI Terminal ##############
MCU_IP_ADDR ?= 192.168.1.69

telnet:
	telnet $(MCU_IP_ADDR) 7777


