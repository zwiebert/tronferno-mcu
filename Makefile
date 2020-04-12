.PHONY: clean all test rebuild http_data print-help

mcus := esp32 esp8266
tgts := all

default: print-help

clean : esp32-test-clean esp32-fullclean esp8266-clean
	make -C test/esp32 clean

print-help:
	@cat docs/make_help.txt


define GEN_RULE
.PHONY: $(1)
$(1): $$(addsuffix -$(1),$(mcus))
endef
$(foreach tgt,$(tgts),$(eval $(call GEN_RULE,$(tgt))))

.PHONY: http_content http_proxy
http_content :
	cd src/components/http_server_content/webapp && make
http_proxy:
	cd src/components/http_server_content/webapp && make proxy

esp8266_build_cmd := make -C src/esp8266
esp8266_tgts_auto := all clean flash app-flash flashinit flasherase spiffs

define GEN_RULE
.PHONY: esp8266-$(1)
esp8266-$(1): http_content
	$(esp8266_build_cmd) $(1)
endef
$(foreach tgt,$(esp8266_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))


####### ESP32 build command ############
PORT ?= /dev/ttyUSB0
esp32_build_opts := -G Ninja -C src/esp32 -p $(PORT)
ifdef V
esp32_build_opts += -v
endif
ifndef BUILD_BASE
esp32_build_dir := src/esp32/build
else
esp32_build_opts += -B $(BUILD_BASE)
esp32_build_dir := $(BUILD_BASE)
endif
esp32_build_cmd := idf.py $(esp32_build_opts)


######### ESP32 Targets ##################
esp32_tgts_auto := menuconfig clean fullclean app flash monitor

.PHONY: esp32-all-force esp32-rebuild
.PHONY: esp32-all esp32-lan esp32-flash esp32-flash-ocd



define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1):
	$(esp32_build_cmd) $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))


esp32-all: http_content
	$(esp32_build_cmd) reconfigure all
esp32-lan: http_content
	env FLAVOR_LAN=1 $(esp32_build_cmd) reconfigure all

########### OpenOCD ###################
esp32_ocd_sh :=  sh $(realpath ./src/esp32/esp32_ocd.sh)

esp32-flash-ocd: http_content
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
esp32-test-$(1): http_content
	make -C test/esp32 $(1)
endef
$(foreach tgt,$(esp32_test_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))


host-test-all:
	make -C test/host all_tests  --no-print-directory




############# TCP Terminal ##############
IPADDR ?= 192.168.1.65

tcpterm:
	socat -d -d -v pty,link=/tmp/ttyVirtual0,raw,echo=0,unlink-close,waitslave tcp:$(IPADDR):7777,forever,reuseaddr&
	gtkterm -p /tmp/ttyVirtual0 -c tcpterm
