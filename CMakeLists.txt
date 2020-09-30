cmake_minimum_required(VERSION 3.16)
option(BUILD_HOST_TESTS "build host test" OFF)

option(BUILD_DOXY "build doxy docs" OFF)

if (BUILD_HOST_TESTS)
project(tfmcu-test)

set(COMPONENT_DIRECTORIES comp/app comp/external/components-mcu/components comp/library)
#list(TRANSFORM COMPONENT_DIRECTORIES PREPEND ${CMAKE_CURRENT_LIST_DIR})
set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -Og -ggdb)

include(comp/external/components-mcu/components/test_host/host_test.cmake)

add_subdirectory(comp/app/app_main)
#add_subdirectory(comp/external/components-mcu/components/test_host)


message("all_libs: ${COMPONENT_LIBS}")
message("all_tests: ${TEST_EXECUTABLES}")

message("all_libs_inc_Dirs: ${COMPONENT_LIBS_INC_DIRS}")

file(WRITE "${CMAKE_BINARY_DIR}/all_libs_dirs.txt" ${COMPONENT_LIBS_DIRS})

set(doxy_input_file "${CMAKE_BINARY_DIR}/doxy_input_file.txt")
file(WRITE ${doxy_input_file} "")
foreach(src ${COMPONENT_LIBS_SRCS})
  file(APPEND ${doxy_input_file} "INPUT += ${src}\n")
endforeach()

foreach(dir ${COMPONENT_LIBS_INC_DIRS})
  foreach(suffix .h .hh .hpp)
    file(GLOB_RECURSE hdr_files "${dir}/*${suffix}")
    foreach(hdr_file ${hdr_files})
      file(APPEND ${doxy_input_file} "INPUT += ${hdr_file}\n")
    endforeach()
  endforeach()
endforeach()

foreach(dir ${COMPONENT_LIBS_DIRS})
  foreach(suffix .h .hh .hpp)
    file(GLOB hdr_files "${dir}/*${suffix}")
    foreach(hdr_file ${hdr_files})
      file(APPEND ${doxy_input_file} "INPUT += ${hdr_file}\n")
    endforeach()
  endforeach()
endforeach()

endif()

if (BUILD_DOXY)
 message("config: ${DOXY_CONFIG}")

endif()
