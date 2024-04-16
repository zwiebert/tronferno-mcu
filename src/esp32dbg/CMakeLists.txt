# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "tronferno-mcu")

file(GLOB external_components ${CMAKE_SOURCE_DIR}/../../external/*/components)
set(EXTRA_COMPONENT_DIRS comp/app comp/library comp/config)
list(TRANSFORM EXTRA_COMPONENT_DIRS PREPEND   ${CMAKE_SOURCE_DIR}/../../)
list(APPEND EXTRA_COMPONENT_DIRS ${external_components})


set(COMPONENTS tronferno
               bootloader
               esptool_py
               partition_table
               soc log spi_flash micro-ecc efuse driver esp_rom)

#set environment variables to be used in components
set(comp_compile_feats cxx_std_20)
#set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" )
set(comp_compile_opts "-Wno-error=format=" "-Wno-format" # Workaround IDF-5.0
${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Wno-missing-field-initializers)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

add_compile_definitions(MCU_ESP32 MCU_TYPE="esp32")
add_compile_options("SHELL:-include esp_attr.h") # "SHELL:-include esp_log.h")
#add_compile_options(-include esp_log.h)
#add_compile_definitions(MQTT_SUPPORTED_FEATURE_TRANSPORT_ERR_REPORTING)

if(DEFINED ENV{DISTRO})
add_compile_definitions(DISTRIBUTION)
endif()
if(DEFINED ENV{FLAVOR_LAN})
add_compile_definitions(FLAVOR_LAN)
endif()

function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

project(tronferno-mcu esptool_py)


include(CTest)
