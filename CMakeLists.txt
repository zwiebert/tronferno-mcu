# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(BUILD_DOXY "build doxy docs" OFF)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)


if("${PRESET_NAME}" STREQUAL "host-test" OR ${BUILD_HOST_TESTS} STREQUAL "ON")


set(TARGET_PLATFORM_FILTER "host/.*")

project(tronferno-mcu-tests)

# delete this folder in build, or a second configure will break the build (XXX)
execute_process(COMMAND bash -c "rm -r ${PROJECT_BINARY_DIR}/comp")
# copy the config from config subdirectory. TODO: build config here instead of root-makefile
if(${BUILD_HOST_TESTS})
  message("configure host tests")
  add_custom_target(HOST_TEST_SDKCONFIG COMMAND make flavor=host-test test.cm.configure
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.h DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.cmake DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/configs/host-test/config/sdkconfig.json DESTINATION ${PROJECT_BINARY_DIR}/config)
endif()



file(GLOB external_components ${CMAKE_CURRENT_LIST_DIR}/external/*/components ${CMAKE_CURRENT_LIST_DIR}/external/*/external/*/components)
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/components ${external_components})

message("external_components: ${external_components}")

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts -O0 -ggdb  # -fanalyzer
)

if(${GPROF})
  add_link_options(-pg)
  add_compile_options(-pg)
endif()

if(ASAN)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address -fstack-protector-all)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/external/host-test-idf/components/test_host/host_test.cmake)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/components/tronferno ${CMAKE_BINARY_DIR}/${BIN_COMP_ROOT}/tronferno)

else()

if("${PRESET_NAME}" STREQUAL "")
    set(CACHE{SDKCONFIG_DEFAULTS} VALUE "configs/esp-idf/esp32-debug/sdkconfig.defaults" )
    set(CACHE{SDKCONFIG} VALUE "configs/esp-idf/esp32-debug/sdkconfig" )
endif()

set(PROJECT_NAME "tronferno-mcu")

option(LINKER_DEMANGLE "set linker option --demangle" OFF)


file(GLOB external_components ${CMAKE_CURRENT_LIST_DIR}/external/*/components ${CMAKE_CURRENT_LIST_DIR}/external/*/external/*/components)
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/components ${external_components})

set(COMPONENTS tronferno
               bootloader
               esptool_py
               partition_table
               soc log spi_flash micro-ecc efuse esp_rom)

#set environment variables to be used in components
set(comp_compile_feats cxx_std_20)
#set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" )
set(comp_compile_opts "-Wno-error=format=" "-Wno-format" # Workaround IDF-5.0
 ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-fpermissive>"
 "$<$<COMPILE_LANGUAGE:C,CXX>:-include>" "$<$<COMPILE_LANGUAGE:C,CXX>:esp_attr.h>"
 -Wno-error=type-limits
 -Wno-error=switch
 -Wno-missing-field-initializers)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

add_compile_definitions(MCU_ESP32 MCU_TYPE="esp32")
#add_compile_definitions(MQTT_SUPPORTED_FEATURE_TRANSPORT_ERR_REPORTING)

if(DEFINED ENV{DISTRO})
add_compile_definitions(DISTRIBUTION)
endif()
if(DEFINED ENV{FLAVOR_LAN})
add_compile_definitions(FLAVOR_LAN)
endif()

if(${LINKER_DEMANGLE})
  add_link_options(-Wl,--demangle)
endif()

function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

project(tronferno-mcu esptool_py)


include(CTest)


endif()
