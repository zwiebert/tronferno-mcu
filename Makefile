.PHONY: clean all rebuild http_data

mcus := esp32 esp8266 
tgts := all clean

default: esp32-app

define GEN_RULE
.PHONY: $(1)
$(1): $$(addsuffix -$(1),$(mcus))
endef
$(foreach tgt,$(tgts),$(eval $(call GEN_RULE,$(tgt))))

http_data :
	cd src/net/content && $(MAKE)
	
esp8266_build_cmd := make -C src/esp8266
esp8266_tgts_auto := all clean flash app-flash flashinit flasherase spiffs

define GEN_RULE
.PHONY: esp8266-$(1)
esp8266-$(1): http_data
	$(esp8266_build_cmd) $(1)
endef
$(foreach tgt,$(esp8266_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

ifdef BUILD_DIR_BASE
esp32_build_dir := -B $(BUILD_DIR_BASE)
endif

esp32_build_cmd := idf.py -C src/esp32 $(esp32_build_dir)
esp32_tgts_auto := menuconfig clean app

.PHONY: esp32-all-force esp32-rebuild
.PHONY: esp32-all esp32-lan esp32-flash



define GEN_RULE
.PHONY: esp32-$(1)
esp32-$(1): http_data
	$(esp32_build_cmd) $(1)
endef
$(foreach tgt,$(esp32_tgts_auto),$(eval $(call GEN_RULE,$(tgt))))

esp32-all: http_data
	$(esp32_build_cmd) reconfigure all
esp32-lan: http_data
	env FLAVOR_LAN=1 $(esp32_build_cmd) reconfigure all
esp32-flash: http_data
	$(esp32_build_cmd) -p /dev/ttyUSB0 flash

