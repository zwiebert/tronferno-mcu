THIS_ROOT := $(realpath .)
JSHINT := $(THIS_ROOT)/tfmcu/node_modules/jshint/bin/jshint
JS_SRC_DIR := $(THIS_ROOT)/tfmcu/src
JS_BLD_DIR := $(THIS_ROOT)/tfmcu/build


SRC_DIRS := $(addprefix $(THIS_ROOT)/tfmcu/,. src src/store src/net src/styles)

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

SED_OPTS_HTML =  -e 's/tfmcu_dev.js/\/tfmcu.js/'
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

bundle_out = build/tfmcu.js build/tfmcu.js.map build/tfmcu.css build/tfmcu.css.map build/tfmcu.html
bundle_files = $(JS_BLD_DIR)/tfmcu.js $(JS_BLD_DIR)/tfmcu.js.map $(JS_BLD_DIR)/tfmcu.css $(JS_BLD_DIR)/tfmcu.css.map 

.PHONY: bundle
all: build/tfmcu.html build/tfmcu.js



$(addprefix tfmcu/build_dev/tfmcu,.js .css .js.map .css.map): $(SRC_ALL) tfmcu/node_modules
	cd tfmcu && env buildTarget=DEV npm run build
$(addprefix tfmcu/build/tfmcu,.js .css .js.map .css.map): $(SRC_ALL) tfmcu/node_modules
	cd tfmcu && env buildTarget=PROD npm run build

build/tfmcu.%: tfmcu/build/tfmcu.%
	cp -p $< $@  

build/tfmcu.html: build build/tfmcu.css ./tfmcu_dev.html
$(bundle_out): build

tfmcu/node_modules:
	test-d tfmcu/node_modules || cd tfmcu && npm install

.PHONY: clean
clean:
	-rm build/* tfmcu/build/*

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
