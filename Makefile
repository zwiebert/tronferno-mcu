.PHONY: clean all rebuild 

mcus := esp8266 atmega328
tgts := clean all rebuild flash reflash eeprom flashinit flasherase spiffs


define GEN_RULE
.PHONY: $(1)-$(2) $(1)-$(2)-force $(1)-rebuild

$(1)-$(2):
	$$(MAKE) --no-print-directory -f Makefile-$(1).mk $(2)

$(1)-$(2)-force: $(1)-$(2)
	$$(MAKE) --always-make $(1)-$(2)

#$(1)-rebuild: $(1)-all-force


endef

$(foreach mcu,$(mcus),$(foreach tgt,$(tgts),$(eval $(call GEN_RULE,$(mcu),$(tgt)))))

