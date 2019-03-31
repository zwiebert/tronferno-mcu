all: tfmcu_html.c tfmcu_js.c

HTMLMIN ?= ~/.local/bin/htmlmin
JSMIN ?= python -m jsmin


# convert HTML or Javascript to C source const char array
%_html.c : %.html
	xxd -i $< | sed 's/unsigned/const/' > $@
%_js.c : %.js
	xxd -i $< | sed 's/unsigned/const/' > $@

#adapt development HTML file for usage in MCU webserver  
%.html : %_dev.html
	cat $< | sed  -e 's/tfmcu_dev.js/\/tfmcu.js/' \
| ${HTMLMIN} \
>$@ \
&& cat null.txt >>$@

#adapt development Javascript file for usage in MCU webserver  
%.js : %_dev.js
	cat $< | sed -e '/dev-delete-line/d' >/tmp/tfmcu.js \
&& ${JSMIN} /tmp/tfmcu.js >$@ \
&& cat null.txt >>$@


