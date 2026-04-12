.PHONY: clean all test rebuild http_data print-help

flavors = esp32-release esp32-debug host-test

flavor ?= esp32-release

default: help

clean : esp32-test-clean esp32-fullclean
	make -C test/esp32 clean


help:
	@less docs/make_help.txt


####### ESP32 build command ############
PORT ?=/dev/serial/by-path/pci-0000:03:00.0-usb-0:3.1:1.0-port0
V ?= 0
CMAKE_ARGS_GENERATOR ?= -G Ninja



ifneq "$(V)" "0"
esp32_build_opts += -v
endif

env:
	env | grep IDF

THIS_ROOT := $(realpath .)
CMAKE_SRC_PATH :=$(THIS_ROOT)
CONFIG_PATH :=$(THIS_ROOT)/configs/$(flavor)
SDKCONFIG_DEFAULTS := $(CONFIG_PATH)/sdkconfig.defaults
SDKCONFIG := $(CONFIG_PATH)/sdkconfig
BUILD_BASE ?=$(THIS_ROOT)/build/$(flavor)
BUILD_PATH :=$(BUILD_BASE)

tmp_build_dir :=/tmp/tronferno-mcu/build
compile_commands_json_latest :=$(THIS_ROOT)/build/compile_commands.json
sdkconfig_h_latest :=$(THIS_ROOT)/build/sdkconfig.h

esp32_build_args :=$(CMAKE_ARGS_GENERATOR) -C $(CMAKE_SRC_PATH) -B $(BUILD_PATH)  -p $(PORT)  $(esp32_build_opts) \
-D SDKCONFIG_DEFAULTS=$(SDKCONFIG_DEFAULTS) -D SDKCONFIG=$(SDKCONFIG)
esp32_build_cmd :=./idf.sh $(esp32_build_args)
esp32_cmake_cmd :=/usr/bin/cmake -S $(CMAKE_SRC_PATH) -B $(BUILD_PATH) $(CMAKE_ARGS_GENERATOR)

# copy  the last used defines and includes into root/build 
# this is intended if you have all configs in one IDE workspace
# ...otherwise use the files in the actual build directory directly
$(compile_commands_json_latest) $(sdkconfig_h_latest): FORCE
	mkdir -p $(THIS_ROOT)/build/
	cp $(BUILD_BASE)/compile_commands.json $(compile_commands_json_latest)
	cp $(BUILD_BASE)/config/sdkconfig.h $(sdkconfig_h_latest)

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
esp32_ocd_sh :=  "$(CONFIG_PATH)/esp32_ocd.sh" $(CMAKE_SRC_PATH) $(BUILD_PATH)

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
kconfigs:=components/*/Kconfig* external/*/Kconfig
TEST ?= test.weather.test_

include ./host_test_rules.mk


############# Doxygen ###################
doxy_flavors=usr dev api
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy
DOXYFILE_PATH=$(THIS_ROOT)
include doxygen_rules.mk


ext=external/*

	
.PHONY:FORCE

########### github pages ###############
api_html=$(DOXY_BUILD_PATH)/api/html

$(api_html):$(DOXY_BUILD_PATH)/api/input_files
	make doxy-api-build
docs/api:$(api_html)
	-rm -rf docs/api
	-mkdir -p docs
	cp -r $</ $@/

.PHONY: gh_pages

git_current_branch=$(shell git branch --show-current)

gh_pages:
	-git branch -D $@
	git checkout -b $@
	make docs/api
	git add docs/api && git commit -m "Update doc pages"
	git push --set-upstream --force origin $@
	git checkout $(git_current_branch)



############# CLI Terminal ##############
MCU_IP_ADDR ?= 192.168.1.69

telnet:
	telnet $(MCU_IP_ADDR) 7777


