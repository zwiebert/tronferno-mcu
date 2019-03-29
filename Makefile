.PHONY: clean all rebuild http_data

mcus := esp32 esp8266
tgts := clean all rebuild flash app-flash reflash eeprom flashinit flasherase spiffs

http_data :
	cd src/userio/http_data && $(MAKE)

define GEN_RULE
.PHONY: $(1)-$(2) $(1)-$(2)-force $(1)-rebuild

$(1)-$(2):
	$$(MAKE) --directory src/$(1) $(2)

$(1)-$(2)-force: $(1)-$(2)
	$$(MAKE) --always-make $(1)-$(2)

#$(1)-rebuild: $(1)-all-force


endef

$(foreach mcu,$(mcus),$(foreach tgt,$(tgts),$(eval $(call GEN_RULE,$(mcu),$(tgt)))))

