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
build_src     := $(SRC_ALL) $(NJS_DIR)/node_modules


