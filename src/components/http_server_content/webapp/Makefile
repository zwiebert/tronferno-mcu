THIS_ROOT := $(realpath .)
JSHINT := $(THIS_ROOT)/njs/node_modules/jshint/bin/jshint
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

HTMLMIN ?= ~/.local/bin/htmlmin
INCFILES ?= perl ./include_files.pl

SED_OPTS_HTML =  -e 's/wapp_dev.js/\/wapp.js/'
SED_OPTS = -E -e '/dev-delete-line/d' -e '/dev-delete-begin/,/dev-delete-end/d'

buildTarget = buildTarget=DEV
ifdef DISTRO
ifeq ($(DISTRO),1)
SED_OPTS +=  -e '/dev-distro-delete-line/d'  -e '/dev-distro-delete-begin/,/dev-distro-delete-end/d'
buildTarget = buildTarget=PROD
endif
endif

ifndef FLAVOR_LAN
SED_OPTS += -e '/dev-no-lan-delete-line/d'
endif

bundle_out = build/wapp.js build/wapp.js.map build/wapp.css build/wapp.css.map build/wapp.html
bundle_files = $(JS_BLD_DIR)/wapp.js $(JS_BLD_DIR)/wapp.js.map $(JS_BLD_DIR)/wapp.css $(JS_BLD_DIR)/wapp.css.map 

.PHONY: bundle
all: build/wapp.html build/wapp.js



$(addprefix njs/build_dev/wapp,.js .css .js.map .css.map): $(SRC_ALL) njs/node_modules
	cd njs && env buildTarget=DEV npm run build
$(addprefix njs/build/wapp,.js .css .js.map .css.map): $(SRC_ALL) njs/node_modules
	cd njs && env buildTarget=PROD npm run build

build/wapp.%: njs/build/wapp.%
	cp -p $< $@  

build/wapp.html: build build/wapp.css ./wapp_dev.html
$(bundle_out): build

njs/node_modules:
	test-d njs/node_modules || cd njs && npm install

.PHONY: clean
clean:
	-rm build/* njs/build/*

# convert HTML or Javascript to C source const char array
build/%_html.c : %.html
	cat $< null.txt | xxd -i | sed 's/unsigned/const/' > $@

#adapt development HTML file for usage in MCU webserver
build/%.html : %_dev.html
	cat $< | ${INCFILES} | sed  ${SED_OPTS} ${SED_OPTS_HTML} | ${HTMLMIN} >$@

.PHONY: proxy

proxy:
	while true; do ./proxy.js; done
build:
	mkdir -p build
