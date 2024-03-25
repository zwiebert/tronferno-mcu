############# Doxygen ###################
.PHONY: FORCE

define GEN_DOXY_RULE
.PHONY: doxy-$1-build doxy-$1-view

$(DOXY_BUILD_PATH)/$1: FORCE
	mkdir -p $$@

$(DOXY_BUILD_PATH)/$1/doxy_config: $(DOXY_BUILD_PATH)/$1 $(DOXY_BUILD_PATH)/$1/input_files
	cat $(DOXYFILE_PATH)/Doxyfile_$1  >$$@
	cat $(DOXY_BUILD_PATH)/$1/input_files | sed "s~^~INPUT += ~" >>$$@

$(DOXY_BUILD_PATH)/$1/html/index.html: $(DOXY_BUILD_PATH)/$1/doxy_config
	doxygen $$^

doxy-$1-build:  $(DOXY_BUILD_PATH)/$1/input_files $(DOXY_BUILD_PATH)/$1/html/index.html FORCE

doxy-$1-view: doxy-$1-build
	xdg-open $(DOXY_BUILD_PATH)/$1/html/index.html

doxy-$1-auto: doxy-$1-build $(DOXY_BUILD_PATH)/$1/input_files
	make doxy-$1-view&
	while true; do inotifywait --fromfile $(DOXY_BUILD_PATH)/$1/input_files -e modify,create,delete  && make doxy-$1-build; done
endef
$(foreach tgt,$(doxy_flavors),$(eval $(call GEN_DOXY_RULE,$(tgt))))
