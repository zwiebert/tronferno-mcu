cmake_minimum_required(VERSION 3.16)
project(tfmcu-test)

set(COMPONENT_DIRECTORIES comp/app comp/external/components-mcu/components comp/library comp/test)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Og -ggdb)

include(../../comp/external/components-mcu/components/host_test/host_test.cmake)

add_subdirectory(comp/test/test_host)


message("all_libs: ${COMPONENT_LIBS}")
message("all_tests: ${TEST_EXECUTABLES}")

