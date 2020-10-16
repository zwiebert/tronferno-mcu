cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(BUILD_DOXY "build doxy docs" OFF)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)

if (BUILD_HOST_TESTS)
project(tfmcu-test)

add_compile_definitions(USE_JSON) 


set(COMPONENT_DIRECTORIES comp/app comp/external/components-mcu/components comp/library)
#list(TRANSFORM COMPONENT_DIRECTORIES PREPEND ${CMAKE_CURRENT_LIST_DIR})
set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Og -ggdb  # -fanalyzer
)

if(${GPROF})
  add_link_options(-pg)
  add_compile_options(-pg)
endif()

if(ASAN)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address -fstack-protector-all)
endif()

include(comp/external/components-mcu/components/test_host/host_test.cmake)

add_subdirectory(comp/app/app_main)
#add_subdirectory(comp/external/components-mcu/components/test_host)


message("all_libs: ${COMPONENT_LIBS}")
message("all_tests: ${TEST_EXECUTABLES}")

doxy_create_input()

endif()

if (BUILD_DOXY)
 message("config: ${DOXY_CONFIG}")

endif()
