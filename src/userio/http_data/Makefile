
%_html.c : %.html
	xxd -i $< | sed 's/unsigned/const/' > $@
%_js.c : %.js
	xxd -i $< | sed 's/unsigned/const/' > $@

tfmcu.% : tfmcu_dev.%
	cat $< | sed -e '/dev-delete-line/d' \
-e 's/tfmcu_dev.js/\/tfmcu.js/' \
>$@ \
&& echo '\0' >>$@

all: tfmcu_html.c tfmcu_js.c

