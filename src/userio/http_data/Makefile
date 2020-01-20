all: tfmcu_html.c tfmcu_js.c

HTMLMIN ?= ~/.local/bin/htmlmin
JSMIN ?= python -m jsmin

SED_OPTS_HTML =  -e 's/tfmcu_dev.js/\/tfmcu.js/' -e '/dev-delete-line/d'
SED_OPTS_JS = -e '/dev-delete-line/d'

ifdef DISTRO
ifeq ($(DISTRO),1)
SED_OPTS_HTML +=  -e '/dev-distro-delete-line/d'
SED_OPTS_JS +=  -e '/dev-distro-delete-line/d'
endif
endif

# convert HTML or Javascript to C source const char array
%_html.c : %.html
	xxd -i $< | sed 's/unsigned/const/' > $@
%_js.c : %.js
	xxd -i $< | sed 's/unsigned/const/' > $@

#adapt development HTML file for usage in MCU webserver
%.html : %_dev.html
	cat $< | sed  ${SED_OPTS_HTML} \
| ${HTMLMIN} \
>$@ \
&& cat null.txt >>$@

#adapt development Javascript file for usage in MCU webserver
%.js : %_dev.js
	cat $< | sed ${SED_OPTS_JS} >/tmp/tfmcu.js \
&& ${JSMIN} /tmp/tfmcu.js >$@ \
&& cat null.txt >>$@
