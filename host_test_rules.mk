# Host testing rules

######################   public rules ##########################################
.PHONY: host-test-all host-test-configure host-test-build host-test-ctest host-test-ctest-regex


host-test-all:
	make flavor=host-test test.cm.configure_no_kconfgen test.cm.ctest
host-test-fullclean:
	make flavor=host-test test.cm.fullclean
host-test-menuconfig:
	make flavor=host-test test.cm.menuconfig
host-test-configure:
	make flavor=host-test test.cm.configure
host-test-build:
	make flavor=host-test test.cm.build
host-test-ctest:
	make flavor=host-test test.cm.ctest
host-test-ctest-regex: #(usage: make host-test-ctest-regex  TEST=my_regex)
	make flavor=host-test test.cm.ctest.regex


######################  private rules ##########################################
.PHONY: test.cm.menuconfig test.cm.configure test.cm.build config test.cm.fullclean FORCE

#### Steps:
# 1. Kconfig.hostestbuild is generated and includes Kconfig files found in external directory
# 2. menuconfig creates .config in  CMAKE_SRC_PATH (where the root CMakeLists.txt is located
# 3. kconfgen create sdkconfig.h, sdkconfig.cmake, sdkconfig.json in subdirectory config in BUILD_PATH
#    ... we save a copy of the config subdirectory in CMAKE_SRC_PATH and add it to git, so we can build withhout konfgen or menuconfig 
# 4. cmake configure creates the Ninja build files
#
# Note: set flavor to host-test (or another cmake/ctest configuration) for these  private rules to work  as intended
####

config_h:=$(BUILD_PATH)/config/sdkconfig.h
config_cmake:=$(BUILD_PATH)/config/sdkconfig.cmake
config_json:=$(BUILD_PATH)/config/sdkconfig.json
_config:=$(CMAKE_SRC_PATH)/.config


test.cm.fullclean:
	rm -fr $(BUILD_PATH)


## create configuration using kconfig menuconfig

$(BUILD_PATH) $(BUILD_PATH)/config: FORCE
	rm -fr $(BUILD_PATH)
	mkdir -p $(BUILD_PATH)/config $(CMAKE_SRC_PATH)/config
Kconfig.hosttestbuild:$(kconfigs)
	ls $(kconfigs) | sed -E -e 's/^/rsource  \"/' -e 's/$$/\"/' >$@
test.cm.menuconfig $(_config):Kconfig.hosttestbuild
	mkdir -p $(CMAKE_SRC_PATH) && cd $(CMAKE_SRC_PATH) && menuconfig $(THIS_ROOT)/Kconfig.hosttestbuild && touch $(_config)
$(config_h) $(config_cmake) $(config_json) config: $(_config)
	python -m kconfgen  --kconfig $(THIS_ROOT)/Kconfig.hosttestbuild --config $(_config) \
		--output header $(config_h) --output cmake $(config_cmake) --output json $(config_json)
	mkdir -p $(CMAKE_SRC_PATH)/config && cp $(config_h) $(config_cmake) $(config_json) $(CMAKE_SRC_PATH)/config


## configure with Cmake, build with Ninja, run with Ctest

test.cm.configure:$(BUILD_PATH)/config $(config_h) $(config_cmake) $(config_json)
	cmake -D BUILD_HOST_TESTS=ON -B $(BUILD_PATH)  -S  $(CMAKE_SRC_PATH) -G Ninja
test.cm.configure_no_kconfgen: $(BUILD_PATH)/config $(CMAKE_SRC_PATH)/config/sdkconfig.h $(CMAKE_SRC_PATH)/config/sdkconfig.cmake
	cp $(CMAKE_SRC_PATH)/config/sdkconfig.h $(CMAKE_SRC_PATH)/config/sdkconfig.cmake $(BUILD_PATH)/config/
	cmake  -D BUILD_HOST_TESTS=ON  -B $(BUILD_PATH)  -S  $(CMAKE_SRC_PATH) -G Ninja
test.cm.build:
	ninja -C $(BUILD_PATH) 
	make $(compile_commands_json_latest)
test.cm.ctest: test.cm.build
	cd  $(BUILD_PATH) && ctest --output-on-failure
test.cm.ctest.regex: test.cm.build
	(cd  $(BUILD_PATH) && ctest --output-on-failure -R "$(TEST)")

