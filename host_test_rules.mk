.PHONY: test.cm.configure test.cm.build config

Kconfig.hosttestbuild:$(kconfigs)
	ls $(kconfigs) | sed -E -e 's/^/rsource  \"/' -e 's/$$/\"/' >$@

menuconfig $(_config):Kconfig.hosttestbuild
	mkdir -p $(config_dir) && cd $(config_dir) && menuconfig $(THIS_ROOT)/Kconfig.hosttestbuild

$(config_h) $(config_cmake) config: $(_config)
	python -m kconfgen  --kconfig $(THIS_ROOT)/Kconfig.hosttestbuild --config $(_config) --output header $(config_h) --output cmake $(config_cmake)
	cp $(config_h) $(config_cmake) $(config_dir)/config

test.cm.configure: $(config_h) $(config_cmake)
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)/config
	make $(config_h) $(config_cmake)
	cmake -B $(HOST_TEST_BUILD_PATH) -S  $(HOST_TEST_SRC_PATH) #-G Ninja)

test.cm.configure_no_kconfgen:
	rm -fr $(HOST_TEST_BUILD_PATH)
	mkdir -p $(HOST_TEST_BUILD_PATH)/config
	cp $(config_dir)/config/sdkconfig.h $(config_dir)/config/sdkconfig.cmake $(HOST_TEST_BUILD_PATH)/config/
	cmake -B $(HOST_TEST_BUILD_PATH)  -S  $(HOST_TEST_SRC_PATH) #-G Ninja)

test.cm.build:
	make -C $(HOST_TEST_BUILD_PATH) -k -j  -s --no-print-dir $(make_verbose_opts)

test.cm.ctest: test.cm.build
	cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure

test.cm.ctest.regex: test.cm.build
	(cd  $(HOST_TEST_BUILD_PATH) && ctest --output-on-failure -R "$(TEST)")

host-test-all:
	make -s --no-print-directory  test.cm.configure_no_kconfgen test.cm.ctest


