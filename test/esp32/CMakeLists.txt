# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

list(APPEND EXTRA_COMPONENT_DIRS $ENV{COMP_DIRS})


#set environment variables to be used in components CMakeLists.txt files
set(component_compile_opts "-std=gnu++2a" "-Wno-missing-field-initializers" "-Wno-narrowing")
set(UNIT_TESTING 1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNIT_TESTING -DMCU_ESP32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TESTING -DMCU_ESP32")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(unit-test-app)


add_compile_definitions(MCU_ESP32)
add_compile_definitions(UNIT_TESTING)
if(DEFINED ENV{DISTRO})
add_compile_definitions(DISTRIBUTION)
endif()
if(DEFINED ENV{FLAVOR_LAN})
add_compile_definitions(FLAVOR_LAN)
endif()
