all: tfmcu.html tfmcu.js

HTMLMIN ?= ~/.local/bin/htmlmin
JSMIN ?= python -m jsmin

SED_OPTS_HTML =  -e 's/tfmcu_dev.js/\/tfmcu.js/'
SED_OPTS_JS =
SED_OPTS = -e '/dev-delete-line/d'

ifdef DISTRO
ifeq ($(DISTRO),1)
SED_OPTS +=  -e '/dev-distro-delete-line/d'
endif
endif

ifndef FLAVOR_LAN
SED_OPTS += -e '/dev-no-lan-delete-line/d'
endif


# convert HTML or Javascript to C source const char array
%_html.c : %.html
	xxd -i $< | sed 's/unsigned/const/' > $@
%_js.c : %.js
	xxd -i $< | sed 's/unsigned/const/' > $@

#adapt development HTML file for usage in MCU webserver
%.html : %_dev.html
	cat $< | sed  ${SED_OPTS} ${SED_OPTS_HTML} \
| ${HTMLMIN} \
>$@ \
&& cat null.txt >>$@

#adapt development Javascript file for usage in MCU webserver
%.js : %_dev.js
	cat $< | sed ${SED_OPTS} ${SED_OPTS_JS} >/tmp/tfmcu.js \
&& ${JSMIN} /tmp/tfmcu.js >$@ \
&& cat null.txt >>$@

.PHONY: proxy

proxy:
	while true; do ./proxy.js; done
