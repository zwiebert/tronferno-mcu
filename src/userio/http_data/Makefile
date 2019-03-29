all: tfmcu_html.c tfmcu_js.c

%_html.c : %.html
	xxd -i $< | sed 's/unsigned/const/' > $@
%_js.c : %.js
	xxd -i $< | sed 's/unsigned/const/' > $@

tfmcu.html : tfmcu_dev.html
	cat $< | sed  -e 's/tfmcu_dev.js/\/tfmcu.js/' \
|htmlmin \
>$@ \
&& cat null.txt >>$@

tfmcu.js : tfmcu_dev.js
	cat $< | sed -e '/dev-delete-line/d' >/tmp/tfmcu.js \
&& python -m jsmin /tmp/tfmcu.js >$@ \
&& cat null.txt >>$@


