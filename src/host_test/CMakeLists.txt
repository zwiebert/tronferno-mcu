cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(BUILD_DOXY "build doxy docs" OFF)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)

# esp-idf menuconfig
if (NOT (BUILD_HOST_TESTS OR BUILD_DOXY))
  set(PROJECT_NAME "tronferno-mcu")



file(GLOB external_components ${CMAKE_SOURCE_DIR}/../../external/*/components)
set(EXTRA_COMPONENT_DIRS comp/app comp/library comp/config)
list(TRANSFORM EXTRA_COMPONENT_DIRS PREPEND   ${CMAKE_SOURCE_DIR}/../../)
list(APPEND EXTRA_COMPONENT_DIRS ${external_components})

set(COMPONENTS tronferno)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

function(component_compile_features)
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

project(tronferno-mcu)
endif()

if (BUILD_HOST_TESTS)
project(tfmcu-test)

set(proj_src_dir "${CMAKE_SOURCE_DIR}/../..")

add_compile_definitions(USE_JSON) 
file(GLOB external_components ${CMAKE_SOURCE_DIR}/../../external/*/components)
set(COMPONENT_DIRECTORIES comp/app comp/library comp/config)
list(TRANSFORM COMPONENT_DIRECTORIES PREPEND   ${CMAKE_SOURCE_DIR}/../../)
list(APPEND COMPONENT_DIRECTORIES ${external_components})

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

include(${proj_src_dir}/external/host-test-idf/components/test_host/host_test.cmake)

add_subdirectory(${proj_src_dir}/comp/app/tronferno ${CMAKE_BINARY_DIR}/${BIN_COMP_ROOT}/tronferno)

message("all_inc_dirs: ${COMPONENT_LIBS_INC_DIRS}")
message("all_libs: ${COMPONENT_LIBS}")
message("all_tests: ${TEST_EXECUTABLES}")
message("all_comp_dirs: ${COMPONENT_DIRECTORIES}")
message("all_extra_comp_dirs: ${EXTRA_COMPONENT_DIRS}")

doxy_create_input()

endif() # BUILD_HOST_TESTS

if (BUILD_DOXY)
 message("config: ${DOXY_CONFIG}")
endif()
