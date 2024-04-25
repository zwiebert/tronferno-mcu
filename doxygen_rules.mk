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

$(DOXY_BUILD_PATH)/api/input_files: $(DOXY_BUILD_PATH)/api FORCE
	-rm $@
	for dir in . $(ext) ; do \
		(cd $${dir} && git ls-files README.md 'components/**.h' 'components/**.hh'  | xargs realpath) | fgrep include  >> $@ ; \
	done

$(DOXY_BUILD_PATH)/dev/input_files: $(DOXY_BUILD_PATH)/dev FORCE
	-rm $@
	for dir in . $(ext) ; do \
		(cd $${dir} && git ls-files README.md 'components/**.h' 'components/**.hh' 'components/**.c' 'components/**.cc' 'components/**.cpp' | xargs realpath) >> $@ ; \
	done

$(DOXY_BUILD_PATH)/usr/input_files: $(DOXY_BUILD_PATH)/usr FORCE
	echo "" > $@
