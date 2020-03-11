.PHONY: clean all rebuild http_data print-help

mcus := esp32 esp8266
tgts := all

default: print-help

clean : unity-clean esp32-fullclean esp8266-clean

print-help:
	@cat docs/make_help.txt


define GEN_RULE
.PHONY: $(1)
$(1): $$(addsuffix -$(1),$(mcus))
endef
$(foreach tgt,$(tgts),$(eval $(call GEN_RULE,$(tgt))))

.PHONY: http_content http_proxy
http_content :
	cd src/net/content && make
http_proxy:
	cd src/net/content && make proxy

esp8266_build_cmd := make -C src/esp8266
esp8266_tgts_auto := all clean flash app-flash flashinit flasherase spiffs

define GEN_RULE
.PHONY: esp8266-$(1)
esp8266-$(1): http_content
	$(esp8266_build_cmd) $(1)
endef
$(foreach tgt,$(esp8266_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

ifdef BUILD_DIR_BASE
esp32_build_dir := -B $(BUILD_DIR_BASE)
endif

esp32_build_cmd := idf.py -C src/esp32 $(esp32_build_dir)
esp32_tgts_auto := menuconfig clean fullclean app

.PHONY: esp32-all-force esp32-rebuild
.PHONY: esp32-all esp32-lan esp32-flash



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
esp32-flash: http_content
	$(esp32_build_cmd) -p /dev/ttyUSB0 flash


# unit testing
ifdef TEST_BUILD_DIR_BASE
esp32_test_build_dir := -B $(TEST_BUILD_DIR_BASE)
endif

esp32_test_build_cmd := idf.py -C unity $(esp32_test_build_dir)

.PHONY: esp32-test-build esp32-test-clean esp32-test-flash esp32-test-run test-all unity unity-clean

COMPS := $(shell cd src/components && ls)
#COMPS = fernotron_alias fernotron_auto txtio

unity: unity/unity.txt

unity-clean:
	-rm -r ./unity

port ?= /dev/ttyUSB0

unity/unity.txt:
	cp -upr $(IDF_PATH)/tools/unit-test-app unity
	patch -s -p0 < unity.diff
	touch unity/unity.txt
esp32-test-build: unity
	$(esp32_test_build_cmd) -T "$(COMPS)" reconfigure build
esp32-test-clean: unity
	$(esp32_test_build_cmd) clean
esp32-test-flash: unity
	$(esp32_test_build_cmd) --port $(port) flash
esp32-test-run:
	python3 test/run_tests.py $(port)

test-all: esp32-test-build esp32-test-flash esp32-test-run
