autobuild_out := $(addprefix $(JS_BLD_DIR_DEV)/wapp,.js .css .js.map) #.css.map
build_out     := $(addprefix $(JS_BLD_DIR)/wapp,.js .css .js.map) #.css.map


bundle_names  := $(addprefix wapp,.js .js.map .css)
bundle_out    := $(addprefix $(BUILD_DIR)/,$(bundle_names) wapp.html)
bundle_files  := $(addprefix $(JS_BLD_DIR)/,$(bundle_names))
bundle_z      := $(addsuffix .gz,$(bundle_out)) $(addsuffix .br,$(bundle_out))
bundle_c      := $(addsuffix .c,$(bundle_z))


