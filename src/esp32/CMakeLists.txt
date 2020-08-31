# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "tronferno-mcu")

set(EXTRA_COMPONENT_DIRS ../../components-mcu/components ../components)

set(COMPONENTS
    fernotron fernotron_pos fernotron_alias fernotron_cuas fernotron_sep fernotron_auto fernotron_txtio http_server_content

    app userio_app net net_mqtt_app net_http_server debug gpio key_value_store storage cli_app config  txtio misc

	bootloader esptool_py partition_table soc log spi_flash micro-ecc main efuse driver esp_rom  

)

#set environment variables to be used in components CMakeLists.txt files
set(component_compile_opts "-std=gnu++2a" "-Wno-missing-field-initializers" "-Wno-narrowing")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)


add_compile_definitions(MCU_ESP32)
if(DEFINED ENV{DISTRO})
add_compile_definitions(DISTRIBUTION)
endif()
if(DEFINED ENV{FLAVOR_LAN})
add_compile_definitions(FLAVOR_LAN)
endif()
if(DEFINED ENV{CELLAR})
add_compile_definitions(CELLAR)
endif()

project(tronferno-mcu esptool_py)

