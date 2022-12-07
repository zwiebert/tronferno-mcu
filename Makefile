.PHONY: clean all test rebuild http_data print-help

flavors = esp32 esp32wlan esp32lan esp32test

flavor ?= esp32

default: print-help

clean : esp32-test-clean esp32-fullclean esp8266-clean http_clean
	make -C test/esp32 clean


print-help:
	@cat docs/make_help.txt

.PHONY: http_proxy http_clean
http_proxy:
	cd comp/app/app_webapp && make proxy
http_proxy2:
	cd comp/app/app_webapp && make proxy2
http_clean:
	cd comp/app/app_webapp && make clean


####### ESP32 build command ############
PORT ?= /dev/ttyUSB1
port ?= /dev/ttyUSB1

V ?= 0

ifdef V
esp32_build_opts += -v
endif

THIS_ROOT := $(realpath .)
BUILD_BASE ?= $(THIS_ROOT)/build/$(flavor)
esp32_build_dir := $(BUILD_BASE)
esp32_src_dir := $(THIS_ROOT)/src/$(flavor)

esp32_build_cmd := idf.py -G Ninja -C $(esp32_src_dir) -B $(esp32_build_dir)  -p $(PORT)  $(esp32_build_opts)
esp32_cmake_cmd := cmake -S $(esp32_src_dir) -B $(esp32_build_dir) -G Ninja

######### ESP32 Targets ##################
esp32_tgts_auto := menuconfig clean fullclean app flash monitor gdb gdbgui

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



esp32-all:
	$(esp32_build_cmd) reconfigure all


esp32-png: $(esp32_build_dir)/tfmcu.png
esp32-dot: $(esp32_build_dir)/tfmcu.dot

$(esp32_build_dir)/tfmcu.dot: FORCE
	$(esp32_cmake_cmd) --graphviz=$(esp32_build_dir)/tfmcu.dot
	
	
%.png:%.dot
	dot -Tpng -o $@ $<

FORCE:
########### OpenOCD ###################
esp32_ocd_sh :=  sh $(realpath ./src/esp32/esp32_ocd.sh)

esp32-flash-ocd:
	(cd $(esp32_build_dir) && $(esp32_ocd_sh) flash)
esp32-flash-app-ocd:
	(cd $(esp32_build_dir) && $(esp32_ocd_sh) flash_app)
esp32-ocd:
	$(esp32_ocd_sh) server
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

.PHONY: test.cm.configure test.cm.build

test.cm.configure:
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)
	cmake -B $(HOST_TEST_BUILD_PATH) -D BUILD_HOST_TESTS=ON -S $(realpath .) #-G Ninja

cm_build := make -C $(HOST_TEST_BUILD_PATH) -k -j  -s --no-print-dir $(make_verbose_opts)
#cm_build := (cd $(HOST_TEST_BUILD_PATH) && cmake -G Ninja $(THIS_ROOT) &&  ninja -k 0 --extra-verbose $(ninja_verbose_opts))

	
test.cm.build:
	$(cm_build)

test.cm.ctest: test.cm.build
	cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure
	
test.cm.ctest.current: test.cm.build
	(cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure -R "test.app_mqtt.test_")


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
	cd comp/external/components-mcu && git ls-files '*.h' '*.c' '*.hh' '*.cc' '*.cpp' | sed "s~^~INPUT += $(THIS_ROOT)/comp/external/components-mcu/~" >> $@
/tmp/doxy_input_api: FORCE
	git ls-files '*.h' '*.hh' | fgrep include | sed "s~^~INPUT += $(THIS_ROOT)/~" > $@
	cd comp/external/components-mcu && git ls-files '*.h' '*.hh' | fgrep include | sed "s~^~INPUT += $(THIS_ROOT)/comp/external/components-mcu/~" >> $@

/tmp/doxy_input_usr: FORCE
	echo "" > $@
	
doxy-%-view: doxy-%-build FORCE
	xdg-open $(DOXY_BUILD_PATH)/$*/html/index.html
	
	
FORCE:

############# TCP Terminal ##############
IPADDR ?= 192.168.1.65

tcpterm:
	socat -d -d -v pty,link=/tmp/ttyVirtual0,raw,echo=0,unlink-close,waitslave tcp:$(IPADDR):7777,forever,reuseaddr&
	gtkterm -p /tmp/ttyVirtual0 -c tcpterm

	
	####### ESP8266 ######################
esp8266_build_cmd := make -C src/esp8266
esp8266_tgts_auto := all clean flash app-flash flashinit flasherase spiffs

define GEN_RULE
.PHONY: esp8266-$(1)
esp8266-$(1):
	$(esp8266_build_cmd) $(1)
endef
$(foreach tgt,$(esp8266_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))
	
