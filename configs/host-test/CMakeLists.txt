cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)
option(CMAKE_EXPORT_COMPILE_COMMANDS "generate compilation database for clang-tidy" ON)
option(BUILD_DOXY "build doxy docs" OFF)
option(ASAN "enable compiler sanitizer" OFF)
option(GPROF "enable gprof" OFF)


set(TARGET_PLATFORM_FILTER "host/.*")

project(tronferno-mcu-tests)

# delete this folder in build, or a second configure will break the build (XXX)
execute_process(COMMAND bash -c "rm -r ${PROJECT_BINARY_DIR}/comp")
# copy the config from config subdirectory. TODO: build config here instead of root-makefile
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/config/sdkconfig.h DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/config/sdkconfig.cmake DESTINATION ${PROJECT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/config/sdkconfig.json DESTINATION ${PROJECT_BINARY_DIR}/config)


file(GLOB external_components ${CMAKE_CURRENT_LIST_DIR}/../../external/*/components ${CMAKE_CURRENT_LIST_DIR}/../../external/*/external/*/components)
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../../components ${external_components})

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

include(${CMAKE_CURRENT_LIST_DIR}/../../external/host-test-idf/components/test_host/host_test.cmake)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../components/tronferno ${CMAKE_BINARY_DIR}/${BIN_COMP_ROOT}/tronferno)

