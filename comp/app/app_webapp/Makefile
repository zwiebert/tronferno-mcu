THIS_ROOT := $(realpath .)
JS_SRC_DIR := $(THIS_ROOT)/njs/src
JS_BLD_DIR := $(THIS_ROOT)/njs/build

SRC_DIRS := $(addprefix $(THIS_ROOT)/njs/,. src src/store src/net src/styles)
SRC_JS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.js))
SRC_ALL += $(SRC_JS)
SRC_SVELTE := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.svelte))
SRC_ALL += $(SRC_SVELTE)
SRC_CSS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.css))
SRC_ALL += $(SRC_CSS)
SRC_SCSS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.css))
SRC_ALL += $(SRC_SCSS)

bundle_out = build/wapp.js build/wapp.js.map build/wapp.css  build/wapp.html #build/wapp.css.map
bundle_files = $(JS_BLD_DIR)/wapp.js $(JS_BLD_DIR)/wapp.js.map $(JS_BLD_DIR)/wapp.css #$(JS_BLD_DIR)/wapp.css.map
bundle_c = build/wapp.html.gz.c build/wapp.js.gz.c build/wapp.js.map.gz.c build/wapp.js.map.br.c build/wapp.css.gz.c #build/wapp.css.map.gz.c build/wapp.css.map.br.c

.PHONY: bundle
all: $(bundle_c)

dist = 

$(addprefix njs/build_dev/wapp,.js .css .js.map .css.map): $(SRC_ALL) njs/node_modules
	cd njs && env NODE_ENV=development npm run build
$(addprefix njs/build/wapp,.js .css .js.map .css.map): $(SRC_ALL) njs/node_modules
	cd njs && env NODE_ENV=production npm run build

build/wapp.%: njs/build/wapp.%
	cp -p $< $@  

%.gz:%
	gzip --keep --force --best $< 
%.br:%
	brotli --keep --force --best $<
build/%.c:build/%
	xxd -i  $< | sed 's/unsigned/const/' > $@
	
build/wapp.html: build build/wapp.css ./wapp_dev.html
	cat ./wapp_dev.html >build/wapp.html
	
$(bundle_out): build

njs/node_modules:
	test-d njs/node_modules || cd njs && npm install

.PHONY: clean
clean:
	-rm build/* njs/build/*

.PHONY: proxy

proxy:
	while true; do ./proxy.js; done
proxy2:
	while true; do ./proxy2.js; done
build:
	mkdir -p build
