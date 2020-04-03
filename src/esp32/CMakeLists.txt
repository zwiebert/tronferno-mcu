# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "tronferno-mcu")

set(EXTRA_COMPONENT_DIRS ../../components-mcu/components ../../../components-mcu/components ../components)

set(COMPONENTS
    fernotron fernotron_pos fernotron_alias fernotron_cuas fernotron_sep fernotron_auto fernotron_txtio

    app userio_app net net_mqtt_app net_http_server debug gpio key_value_store storage cli_app config  txtio misc

	bootloader esptool_py partition_table soc log spi_flash micro-ecc main efuse driver esp_rom  

)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(tronferno-mcu esptool_py)

