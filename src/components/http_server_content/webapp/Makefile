all: tfmcu.html tfmcu.js

HTMLMIN ?= ~/.local/bin/htmlmin
JSMIN ?= python -m jsmin

SED_OPTS_HTML =  -e 's/tfmcu_dev.js/\/tfmcu.js/'
SED_OPTS_JS =
SED_OPTS = -E -e '/dev-delete-line/d'

ifdef DISTRO
ifeq ($(DISTRO),1)
SED_OPTS +=  -e '/dev-distro-delete-line/d' -e '/^\s*\bdbLog\b.*;/d'
endif
endif

ifndef FLAVOR_LAN
SED_OPTS += -e '/dev-no-lan-delete-line/d'
endif

outfiles := tfmcu.html tfmcu.js tfmcu_html.c tfmcu_js.c

.PHONY: clean
clean:
	-rm $(outfiles)

# convert HTML or Javascript to C source const char array
%_html.c : %.html
	cat $< null.txt | xxd -i | sed 's/unsigned/const/' > $@
%_js.c : %.js
	cat $< null.txt | xxd -i | sed 's/unsigned/const/' > $@

#adapt development HTML file for usage in MCU webserver
%.html : %_dev.html
	cat $< | sed  ${SED_OPTS} ${SED_OPTS_HTML} | ${HTMLMIN} >$@

#adapt development Javascript file for usage in MCU webserver
%.js : %_dev.js
	cat $< | sed ${SED_OPTS} ${SED_OPTS_JS} >/tmp/tfmcu.js && ${JSMIN} /tmp/tfmcu.js >$@

.PHONY: proxy

proxy:
	while true; do ./proxy.js; done
