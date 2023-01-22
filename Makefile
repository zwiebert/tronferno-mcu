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
PORT ?= /dev/ttyUSB1
port ?= /dev/ttyUSB1

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
esp32_tgts_auto := menuconfig clean fullclean app flash monitor gdb gdbgui reconfigure

.PHONY: esp32-all-force esp32-rebuild
.PHONY: esp32-all esp32-flash esp32-flash-ocd
.PHONY: esp32-dot
.PHONY: FORCE

define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1):
	$(esp32_build_cnd) $(1) 
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))



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
########### OpenOCD ###################
esp32_ocd_sh :=  $(realpath ./src/esp32/esp32_ocd.sh) $(esp32_src_dir) $(esp32_build_dir)

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

.PHONY: test.cm.configure test.cm.build

test.cm.configure:
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)/config
	cp $(THIS_ROOT)/src/host_test/sdkconfig.h $(THIS_ROOT)/src/host_test/sdkconfig.cmake $(HOST_TEST_BUILD_PATH)/config/
	cmake -B $(HOST_TEST_BUILD_PATH) -D BUILD_HOST_TESTS=ON -S  $(HOST_TEST_SRC_PATH) #-G Ninja)

cm_build := make -C $(HOST_TEST_BUILD_PATH) -k -j  -s --no-print-dir $(make_verbose_opts)
#cm_build := (cd $(HOST_TEST_BUILD_PATH) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --verbose $(ninja_verbose_opts))

	
test.cm.build:
	$(cm_build)

test.cm.ctest: test.cm.build
	cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure
	
test.cm.ctest.current: test.cm.build
	(cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure -R "test.config_kvs.test_")


host-test-all:
	make -s --no-print-directory  test.cm.configure test.cm.ctest


############# Doxygen ###################
DOXY_BUILD_PATH=$(THIS_ROOT)/build/doxy

.PHONY: FORCE
.PHONY: doxy-usr-view  doxy-dev-view  doxy-api-view
.PHONY: doxy-usr-build doxy-dev-build doxy-api-build 

$(DOXY_BUILD_PATH)/%/html/index.html: /tmp/doxy_%_file 
	mkdir -p $(dir $@)
	doxygen $^
	

doxy-usr-build: $(DOXY_BUILD_PATH)/usr/html/index.html FORCE
doxy-usr-view: doxy-usr-build
	xdg-open $(DOXY_BUILD_PATH)/usr/html/index.html

doxy-dev-build: $(DOXY_BUILD_PATH)/dev/html/index.html FORCE
doxy-dev-view: doxy-dev-build
	xdg-open $(DOXY_BUILD_PATH)/dev/html/index.html
	
doxy-api-build: $(DOXY_BUILD_PATH)/api/html/index.html FORCE
doxy-api-view: doxy-api-build
	xdg-open $(DOXY_BUILD_PATH)/api/html/index.html


/tmp/doxy_%_file: ./Doxyfile_% /tmp/doxy_input_%
	cat $^ > $@

/tmp/doxy_input_dev: FORCE
	git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | sed "s~^~INPUT += $(THIS_ROOT)/~" > $@
	cd comp/components-mcu && git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | sed "s~^~INPUT += $(THIS_ROOT)/comp/components-mcu/~" >> $@
/tmp/doxy_input_api: FORCE
	git ls-files '*.h' '*.hh' | fgrep include | sed "s~^~INPUT += $(THIS_ROOT)/~" > $@
	cd comp/components-mcu && git ls-files '*.h' '*.hh' | fgrep include | sed "s~^~INPUT += $(THIS_ROOT)/comp/components-mcu/~" >> $@

/tmp/doxy_input_usr: FORCE
	echo "" > $@
	
doxy-%-view: doxy-%-build FORCE
	xdg-open $(DOXY_BUILD_PATH)/$*/html/index.html
	
	
FORCE:

############# CLI Terminal ##############
MCU_IP_ADDR ?= 192.168.1.69

telnet:
	telnet $(MCU_IP_ADDR) 7777
