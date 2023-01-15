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



SRC_DIRS      := $(addprefix $(THIS_ROOT)/njs/,. src src/store src/net src/styles)
SRC_JS        := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.js))
SRC_ALL       += $(SRC_JS)
SRC_SVELTE    := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.svelte))
SRC_ALL       += $(SRC_SVELTE)
SRC_CSS       := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.css))
SRC_ALL       += $(SRC_CSS)
SRC_SCSS      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.css))
SRC_ALL       += $(SRC_SCSS)
SRC_ALL       :=  $(shell find njs/src -name '*.svelte' -or -name '*.js' -or -name '*.css')

autobuild_src := $(SRC_ALL) njs/node_modules
autobuild_out := $(addprefix $(JS_BLD_DIR_DEV)/wapp,.js .css .js.map) #.css.map
build_src     :=  $(SRC_ALL) njs/node_modules 
build_out     := $(addprefix $(JS_BLD_DIR)/wapp,.js .css .js.map) #.css.map


bundle_out    := $(addprefix $(BUILD_DIR)/wapp,.js .js.map .css .html)
bundle_files  := $(addprefix $(JS_BLD_DIR)/wapp,.js .js.map .css)
bundle_z      := $(addprefix $(BUILD_DIR)/wapp,.html.gz .js.gz .js.map.gz .js.map.br .css.gz)
bundle_c      := $(addsuffix .c,$(bundle_z)) #.css.map.gz.c .css.map.br.c

src_dirs      := $(shell find njs/src -type d)
src_files     := $(shell find njs/src -name '*.svelte' -or -name '*.js') ./Makefile



.PHONY: all deb bundle autobuild appbuild FORCE build_out
all: $(bundle_c)
	echo "build target all"

deb:
	echo "bundle_c=$(bundle_c)"
	echo "BUILD_DIR=$(BUILD_DIR)"
	echo "src_files=$(src_files)"
	echo "==================================================="
	echo "build_src=$(build_src)"

autobuild: $(autobuild_out)
appbuild: FORCE
	make $(build_out)
bundle_c: $(bundle_c)
build_out:
	make $(build_out)

$(build_out): $(build_src) $(src_files)
	cd njs && env BUILD_DIR=$(JS_BLD_DIR) NODE_ENV=production npm run build

xxx:
	make $(JS_BLD_DIR)/wapp.js

yyy:
	make $(BUILD_DIR)/wapp.js

	
$(autobuild_out): $(autobuild_src)
	cd njs && env BUILD_DIR=$(JS_BLD_DIR_DEV) NODE_ENV=development npm run autobuild



$(BUILD_DIR)/wapp.%: $(JS_BLD_DIR)/wapp.%
	cp -p "$<" "$@"  

%.gz:%
	gzip --keep --force --best $< 
$(BUILD_DIR)/%.gz:$(BUILD_DIR)/%
	gzip --keep --force --best $< 
$(BUILD_DIR)/%.br:$(BUILD_DIR)/%
	brotli --keep --force --best $<
$(BUILD_DIR)/%.c:$(BUILD_DIR)/%
	(name=$(shell echo "build_$(notdir $<)" | tr "." "_"); \
   echo "const char $${name}[] = {" > $@; \
   hexdump -ve '1/1 "0x%x,"' $< >>$@; \
   echo "}; const unsigned $${name}_len = sizeof $${name};" >> $@)


$(bundle_out): $(BUILD_DIR) $(bundle_files)

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
