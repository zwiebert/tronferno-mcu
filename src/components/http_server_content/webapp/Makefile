all: build build/tfmcu.html build/tfmcu.js

HTMLMIN ?= ~/.local/bin/htmlmin
TERSER ?= terser --source-map

SED_OPTS_HTML =  -e 's/tfmcu_dev.js/\/tfmcu.js/'
SED_OPTS_JS =
SED_OPTS = -E -e '/dev-delete-line/d'

ifdef DISTRO
ifeq ($(DISTRO),1)
SED_OPTS +=  -e '/dev-distro-delete-line/d' -e '/^\s*\bdbLog\b.*;/d'   -e '/dev-distro-delete-begin/,/dev-distro-delete-end/d'
endif
endif

ifndef FLAVOR_LAN
SED_OPTS += -e '/dev-no-lan-delete-line/d'
endif

outfiles := build/tfmcu.html build/tfmcu.js build/tfmcu_html.c build/tfmcu_js.c

.PHONY: clean
clean:
	-rm $(outfiles)

# convert HTML or Javascript to C source const char array
build/%_html.c : %.html
	cat $< null.txt | xxd -i | sed 's/unsigned/const/' > $@
build/%_js.c : %.js
	cat $< null.txt | xxd -i | sed 's/unsigned/const/' > $@

#adapt development HTML file for usage in MCU webserver
build/%.html : %_dev.html
	cat $< | sed  ${SED_OPTS} ${SED_OPTS_HTML} | ${HTMLMIN} >$@

#adapt development Javascript file for usage in MCU webserver
build/%.js : %_dev.js
	cat $< | sed ${SED_OPTS} ${SED_OPTS_JS} | ${TERSER} -o $@

.PHONY: proxy

proxy:
	while true; do ./proxy.js; done
build:
	mkdir -p build