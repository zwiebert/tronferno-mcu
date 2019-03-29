
%_html.c : %.html
	xxd -i $< | sed 's/unsigned/const/' > $@
%_js.c : %.js
	xxd -i $< | sed 's/unsigned/const/' > $@

all: tfmcu_html.c tfmcu_js.c

