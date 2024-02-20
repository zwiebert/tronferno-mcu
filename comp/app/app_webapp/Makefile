#Build webapp with node.js

THIS_ROOT := $(realpath .)

ifeq "x$(BUILD_DIR)" "x"
   BUILD_DIR ?= $(realpath .)/build
   JS_BLD_DIR ?= $(THIS_ROOT)/njs/build
   JS_BLD_DIR_DEV ?= /tmp/tronferno-mcu/njs/build
else
   JS_BLD_DIR ?= $(BUILD_DIR)/njs/build
   JS_BLD_DIR_DEV ?= /tmp/tronferno-mcu/njs/build
endif

JS_SRC_DIR := $(THIS_ROOT)/njs/src
NJS_DIR :=$(THIS_ROOT)/njs


SDKCONFIG_JSON = $(BUILD_DIR)/../config/sdkconfig.json

ifneq ("$(wildcard $(SDKCONFIG_JSON))","")
export SDKCONFIG_JS_DIR = $(BUILD_DIR)
SDKCONFIG_JS = $(SDKCONFIG_JS_DIR)/sdkconfig.js

$(SDKCONFIG_JS): FORCE
	(echo "export const sdkconfig = "; cat $(SDKCONFIG_JSON); echo ";") > $(SDKCONFIG_JS)
	touch -r $(SDKCONFIG_JSON) $(SDKCONFIG_JS)
else
export SDKCONFIG_JS_DIR = $(NJS_DIR)/src/config
SDKCONFIG_JS = $(SDKCONFIG_JS_DIR)/sdkconfig.js 
endif


SRC_DIRS      := $(addprefix $(THIS_ROOT)/njs/,. src src/store src/net src/styles)
SRC_JS        := $(shell find njs/src -name '*.js')
SRC_ALL       += $(SRC_JS)
SRC_SVELTE    := $(shell find njs/src -name '*.svelte')
SRC_ALL       += $(SRC_SVELTE)
SRC_CSS       := $(shell find njs/src -name '*.css')
SRC_ALL       += $(SRC_CSS)
SRC_SCSS      :=$(shell find njs/src -name '*.scss')
SRC_ALL       += $(SRC_SCSS)

BUILDERS      := ./Makefile ./CMakeLists.txt $(wildcard ./njs/*.js) $(wildcard ./njs/*.mjs)

autobuild_src := $(SRC_ALL) $(NJS_DIR)/node_modules
autobuild_out := $(addprefix $(JS_BLD_DIR_DEV)/wapp,.js .css .js.map) #.css.map
build_src     := $(SRC_ALL) $(NJS_DIR)/node_modules 
build_out     := $(addprefix $(JS_BLD_DIR)/wapp,.js .css .js.map) #.css.map


bundle_names  := $(addprefix wapp,.js .js.map .css)
bundle_out    := $(addprefix $(BUILD_DIR)/,$(bundle_names) wapp.html)
bundle_files  := $(addprefix $(JS_BLD_DIR)/,$(bundle_names))
bundle_z      := $(addsuffix .gz,$(bundle_out)) $(addsuffix .br,$(bundle_out))
bundle_c      := $(addsuffix .c,$(bundle_z))





.PHONY: all deb bundle autobuild appbuild FORCE build_out
all: $(bundle_c)
	echo "build target all"

deb:
	echo "bundle_c=$(bundle_c)"
	echo "BUILD_DIR=$(BUILD_DIR)"
	echo "src_files=$(SRC_ALL)"
	echo "==================================================="
	echo "build_src=$(build_src)"

autobuild: $(autobuild_out)
appbuild: FORCE
	make $(build_out)
bundle_c: $(bundle_c)
build_out:
	make $(build_out)

#/usr/local/bin is added to path for npm/node as a workaround because my eclipse adds "/bin" in front for some reason...
# ...remove this later when no longer needed -bertw/2024-02
$(build_out): $(build_src) $(SRC_ALL) $(BUILDERS) $(SDKCONFIG_JS)
	cd njs && PATH=/usr/local/bin:$(PATH) BUILD_DIR=$(JS_BLD_DIR) NODE_ENV=production npm run build

xxx:
	make $(JS_BLD_DIR)/wapp.js

yyy:
	make $(BUILD_DIR)/wapp.js

	
$(autobuild_out): $(autobuild_src) $(BUILDERS) $(SDKCONFIG_JS)
	cd njs && BUILD_DIR=$(JS_BLD_DIR_DEV) NODE_ENV=development npm run autobuild


$(BUILD_DIR)/wapp.html: ./wapp.html
	cp -p "$<" "$@"

$(BUILD_DIR)/wapp.%: $(JS_BLD_DIR)/wapp.%
	cp -p "$<" "$@"  

%.gz:%
	gzip --keep --force --best $< 
$(BUILD_DIR)/%.gz:$(BUILD_DIR)/%
	gzip --keep --force --best $< 
$(BUILD_DIR)/%.br:$(BUILD_DIR)/%
	brotli --keep --force --best $<
$(BUILD_DIR)/%.c:$(BUILD_DIR)/% $(BUILDERS)
	(name=$(shell echo "$(notdir $<)" | tr "." "_"); \
	 echo     >$@ "static const char content[] = {" && \
   hexdump >>$@ -ve '1/1 "0x%X,"' $< && \
   echo    >>$@ "\n\
}; \n\
struct web_content { \n\
  const char *content, *content_encoding; unsigned content_size; \n\
} const $${name}_fm = { \n\
	 .content = content, \n\
	 .content_encoding = \"$(shell echo $(suffix $<) | sed -e s/.gz/gzip/ -e s/.br/br/)\", \n\
	 .content_size = sizeof content, \n\
};" \
   )


$(bundle_out): $(BUILD_DIR) $(bundle_files)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) $(JS_BLD_DIR) $(JS_BLD_DIR_DEV)


$(NJS_DIR)/node_modules:
	test -d $(NJS_DIR)/node_modules || cd $(NJS_DIR) && npm install

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)/* $(JS_BLD_DIR)/* ./build

#HTTP PROXY
#==========
.PHONY: proxy
MCU_IP_ADDR ?= 192.168.1.69
PROXY_TCP_PORT ?= 3000

proxy:
	(while true; do mkdir -p $(JS_BLD_DIR_DEV) && ./proxy.js $(JS_BLD_DIR_DEV) $(MCU_IP_ADDR) $(PROXY_TCP_PORT); done)

build:
	mkdir -p build
