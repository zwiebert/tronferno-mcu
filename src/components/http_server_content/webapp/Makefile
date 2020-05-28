THIS_ROOT := $(realpath .)
JSHINT := $(THIS_ROOT)/tfmcu/node_modules/jshint/bin/jshint
JS_SRC_DIR := $(THIS_ROOT)/tfmcu/src
JS_BLD_DIR := $(THIS_ROOT)/tfmcu/build

HTMLMIN ?= ~/.local/bin/htmlmin
SED_OPTS_HTML =  -e 's/tfmcu_dev.js/\/tfmcu.js/'
SED_OPTS = -E -e '/dev-delete-line/d'

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


all: build bundle build/tfmcu.html

.PHONY: bundle old
bundle: tfmcu/node_modules
	-cd ../../../.. && $(JSHINT) $(JS_SRC_DIR)/*.js
	#cd ../../../.. && $(JSHINT) $(JS_BLD_DIR)/bundle.js
	cd tfmcu && env $(buildTarget) npm run build
	cp $(JS_BLD_DIR)/bundle.min.js build/tfmcu.js

old: build build/tfmcu.html build/tfmcu.js
	cd ../../../.. && jshint src/components/http_server_content/webapp/tfmcu_dev.js

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
	cat $< | sed  ${SED_OPTS} ${SED_OPTS_HTML} | ${HTMLMIN} >$@

.PHONY: proxy

proxy:
	while true; do ./proxy.js; done
build:
	mkdir -p build
