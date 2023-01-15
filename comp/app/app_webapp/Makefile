THIS_ROOT := $(realpath .)



ifeq "x$(BUILD_DIR)" "x"
   BUILD_DIR ?= $(realpath .)/build
   JS_BLD_DIR ?= $(THIS_ROOT)/njs/build
   JS_BLD_DIR_DEV ?= $(THIS_ROOT)/njs/build_dev
else
   JS_BLD_DIR ?= $(BUILD_DIR)/njs/build
   JS_BLD_DIR_DEV ?= $(THIS_ROOT)/njs/build_dev
endif

JS_SRC_DIR := $(THIS_ROOT)/njs/src



SRC_DIRS := $(addprefix $(THIS_ROOT)/njs/,. src src/store src/net src/styles)
SRC_JS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.js))
SRC_ALL += $(SRC_JS)
SRC_SVELTE := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.svelte))
SRC_ALL += $(SRC_SVELTE)
SRC_CSS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.css))
SRC_ALL += $(SRC_CSS)
SRC_SCSS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.css))
SRC_ALL += $(SRC_SCSS)

bundle_out = $(BUILD_DIR)/wapp.js $(BUILD_DIR)/wapp.js.map $(BUILD_DIR)/wapp.css  $(BUILD_DIR)/wapp.html #build/wapp.css.map
bundle_files = $(JS_BLD_DIR)/wapp.js $(JS_BLD_DIR)/wapp.js.map $(JS_BLD_DIR)/wapp.css #$(JS_BLD_DIR)/wapp.css.map
bundle_c = $(BUILD_DIR)/wapp.html.gz.c $(BUILD_DIR)/wapp.js.gz.c $(BUILD_DIR)/wapp.js.map.gz.c $(BUILD_DIR)/wapp.js.map.br.c $(BUILD_DIR)/wapp.css.gz.c #build/wapp.css.map.gz.c build/wapp.css.map.br.c

.PHONY: bundle
all: $(bundle_c)
deb:
	echo "BUILD_DIR=$(BUILD_DIR)"

dist = 

$(addprefix $(JS_BLD_DIR_DEV)/wapp,.js .css .js.map .css.map): $(SRC_ALL) njs/node_modules
	cd njs && env BUILD_DIR=$(JS_BLD_DIR_DEV) NODE_ENV=development npm run build
$(addprefix $(JS_BLD_DIR)/wapp,.js .css .js.map .css.map): $(SRC_ALL) njs/node_modules
	cd njs && env BUILD_DIR=$(JS_BLD_DIR) NODE_ENV=production npm run build

$(BUILD_DIR)/wapp.%: $(JS_BLD_DIR)/wapp.%
	cp -p $< $@  

%.gz:%
	gzip --keep --force --best $< 
%.br:%
	brotli --keep --force --best $<
$(BUILD_DIR)/%.c:$(BUILD_DIR)/%
	xxd -i  $< | sed 's/unsigned/const/' > $@
	
$(BUILD_DIR)/wapp.html: $(BUILD_DIR) $(BUILD_DIR)/wapp.css ./wapp_dev.html
	cat ./wapp_dev.html >$(BUILD_DIR)/wapp.html
	
$(bundle_out): $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) $(JS_BLD_DIR) $(JS_BLD_DIR_DEV)

njs/node_modules:
	test -d njs/node_modules || cd njs && npm install

.PHONY: clean
clean:
	-rm $(BUILD_DIR)/* $(JS_BLD_DIR)/*

#HTTP PROXY
#==========
.PHONY: proxy
MCU_IP_ADDR ?= 192.168.1.69
PROXY_TCP_PORT ?= 3000

proxy:
	(while true; do ./proxy.js $(JS_BLD_DIR_DEV) $(MCU_IP_ADDR) $(PROXY_TCP_PORT); done)

build:
	mkdir -p build
