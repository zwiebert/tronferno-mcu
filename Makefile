.PHONY: clean all rebuild http_data

mcus := esp8266
tgts := clean all rebuild flash app-flash reflash eeprom flashinit flasherase spiffs

#default rule
esp32-all:

all: esp32-all esp8266-all
clean: esp32-clean esp8266-clean



http_data :
	cd src/userio/http_data && $(MAKE)

define GEN_RULE
.PHONY: $(1)-$(2) $(1)-$(2)-force $(1)-rebuild

$(1)-$(2): http_data
	$$(MAKE) --directory src/$(1) $(2)

$(1)-$(2)-force: $(1)-$(2)
	$$(MAKE) --always-make $(1)-$(2)

#$(1)-rebuild: $(1)-all-force


endef

$(foreach mcu,$(mcus),$(foreach tgt,$(tgts),$(eval $(call GEN_RULE,$(mcu),$(tgt)))))


.PHONY: esp32-all esp32-all-force esp32-rebuild

esp32-all: http_data
	idf.py -C src/esp32 reconfigure all

esp32-lan: http_data
	env FLAVOR_LAN=1 idf.py -C src/esp32 reconfigure all

esp32-flash: http_data
	idf.py -C src/esp32 flash

esp32-clean: http_data
	idf.py -C src/esp32 clean

