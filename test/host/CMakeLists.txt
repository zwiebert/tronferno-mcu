cmake_minimum_required(VERSION 3.16)

set(COMPONENT_DIRECTORIES comp/app comp/external/components-mcu/components comp/library comp/test)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>" -O2)

project(tfmcu-test)

#find_package(unity PATHS comp/test/test_host/unity)
#add_library(unity STATIC comp/test/test_host/unity/src/unity.c)


add_compile_definitions(HOST TEST_HOST TEST HOST_TESTING UNITY_SUPPORT_TEST_CASES)
set(TEST_HOST true)
set(UNIT_TESTING true)

include(CTest)

function(my_include_from_list)
  cmake_parse_arguments(MY "" "ACCESS;TAG;COMP" "INC_LIST" "${ARGN}")
  # message("@inc_from_list: TAG:${MY_TAG}, COMP: ${MY_COMP}, ACCESS: ${MY_ACCESS},  INC_LIST: ${MY_INC_LIST}")
  foreach(inc_dir ${MY_INC_LIST})
    # message("${COMPONENT_LIB}: include: ${inc_dir} ")
    target_include_directories("${MY_COMP}" "${MY_ACCESS}" "${inc_dir}")
  endforeach()
endfunction()

function(my_comp_add_includes)
  cmake_parse_arguments(MY "" "COMP;ACCESS;MAX_DEPTH;CURRENT_DEPTH" "COMP_LIST;SEEN_LIST" "${ARGN}")
  if(NOT DEFINED MY_CURRENT_DEPTH)
    set(MY_CURRENT_DEPTH 0)
  endif()
  if(NOT DEFINED MY_MAX_DEPTH)
    set(MY_MAX_DEPTH 50)
  endif()

  # message("@comp_add_inc: curr_depth: ${MY_CURRENT_DEPTH}: comps: ${MY_COMP_LIST} seen: ${MY_SEEN_LIST}")

  math(EXPR MY_CURRENT_DEPTH "${MY_CURRENT_DEPTH} + 1")
  if(${MY_CURRENT_DEPTH} GREATER ${MY_MAX_DEPTH})
    message("error: components nested too deep")
    return()
  endif()

  foreach(req ${MY_COMP_LIST})
    my_include_from_list(
      COMP
      ${MY_COMP}
      TAG
      "(depth: ${MY_CURRENT_DEPTH}, ${req})"
      ACCESS
      ${MY_ACCESS}
      INC_LIST
      ${${req}_INCLUDE_DIRS})

    list(FIND MY_SEEN_LIST ${req} seen_idx)
    if(${seen_idx} EQUAL -1)
      list(APPEND MY_SEEN_LIST ${req})
      my_comp_add_includes(
        MAX_DEPTH
        ${MY_MAX_DEPTH}
        CURRENT_DEPTH
        ${MY_CURRENT_DEPTH}
        SEEN_LIST
        "${MY_SEEN_LIST}"
        COMP
        ${MY_COMP}
        ACCESS
        ${MY_ACCESS}
        COMP_LIST
        ${${req}_REQUIRES})
    endif()
  endforeach()

endfunction()

set(test_this 0)
# set(MY_SEEN_LIST a b c)
set(req b)
list(FIND MY_SEEN_LIST "${req}" seen_idx)
message("si: ${seen_idx}")

if(${test_this})
  message("start testing this")
  set(test_comp_list app_main)

  my_comp_add_includes(ACCESS "PUBLIC" COMP_LIST ${test_comp_list})

  return()
endif()

macro(my_filter_sources)
   list(FILTER ${ARGV0} EXCLUDE "esp32/*")
endmacro()

function(set_comp_vars)
  cmake_parse_arguments(MY "" "NAME" "" "${ARGN}")
  #message("MY_NAME: ${MY_NAME}")
  set("${MY_NAME}_INCLUDE_DIRS"
      ${INC_PATHS}
      CACHE INTERNAL "${MY_NAME}_INCLUDE_DIRS")

  set("${MY_NAME}_PRIV_INCLUDE_DIRS"
      ${PRIV_INC_PATHS}
      CACHE INTERNAL "${MY_NAME}_PRIV_INCLUDE_DIRS")

  set("${MY_NAME}_REQUIRES"
      ${__REQUIRES}
      CACHE INTERNAL "${MY_NAME}_REQUIRES")

  set("${MY_NAME}_PRIV_REQUIRES"
      ${__PRIV_REQUIRES}
      CACHE INTERNAL "${MY_NAME}_PRIV_REQUIRES")
endfunction()

macro(idf_component_register)
  set(options)
  set(single_value KCONFIG KCONFIG_PROJBUILD)
  set(multi_value
      SRCS
      SRC_DIRS
      EXCLUDE_SRCS
      INCLUDE_DIRS
      PRIV_INCLUDE_DIRS
      LDFRAGMENTS
      REQUIRES
      PRIV_REQUIRES
      REQUIRED_IDF_TARGETS
      EMBED_FILES
      EMBED_TXTFILES)
  cmake_parse_arguments(_ "${options}" "${single_value}" "${multi_value}" "${ARGN}")
  set(__component_priv_requires "${__PRIV_REQUIRES}")
  set(__component_requires "${__REQUIRES}")
  set(__component_kconfig "${__KCONFIG}")
  set(__component_kconfig_projbuild "${__KCONFIG_PROJBUILD}")
  set(__component_registered 1)

  message("orig_sources: ${__SRCS}")
  list(FILTER __SRCS EXCLUDE REGEX "^esp[0-9]+/.*")
  message("fil_sources: ${__SRCS}")

  set(host_dir ${CMAKE_CURRENT_LIST_DIR}/host)
  file(GLOB host_files "${host_dir}/*.cc" "${host_dir}/*.cpp" "${host_dir}/*.c")
  set(__SRCS ${__SRCS} ${host_files})
  message("###############host_files: ${host_files}")
  message("host_sources: ${__SRCS}")

  list(TRANSFORM __INCLUDE_DIRS PREPEND "${CMAKE_CURRENT_LIST_DIR}/" OUTPUT_VARIABLE INC_PATHS)
  list(TRANSFORM _PRIV_INCLUDE_DIRS PREPEND "${CMAKE_CURRENT_LIST_DIR}/" OUTPUT_VARIABLE PRIV_INC_PATHS)
  list(APPEND __PRIV_REQUIRES test_host)

  # return()

  get_filename_component(COMPONENT_LIB ${CMAKE_CURRENT_LIST_DIR} NAME)

  if("test" STREQUAL "${COMPONENT_LIB}")

    message("%%%%%%%%%%%% ${CMAKE_CURRENT_LIST_DIR} %%%%%%%%%%%%%%%%%%%%%% srcs: ${__SRC_DIRS}")

   

    foreach(test_dir ${__SRC_DIRS})
      file(GLOB test_files "${test_dir}/test_*.cc" "${test_dir}/test_*.cpp" "${test_dir}/test_*.c")
      set(__SRCS ${__SRCS} ${test_files})
    endforeach()

    foreach(test_file ${__SRCS})
      message("file: ${test_file}")
      get_filename_component(test_name ${test_file} NAME_WE)
      add_executable(${test_name} ${test_file})
      target_compile_options(${test_name} PRIVATE ${comp_compile_opts})
      target_compile_features(${test_name} PRIVATE ${comp_compile_feats})
      #target_link_options(${test_name} PRIVATE "-flto" "-O2")
      #target_link_libraries(${test_name} PRIVATE unity)
      add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
      set_comp_vars(NAME ${test_name})
    set(TEST_EXECUTABLES
      "${TEST_EXECUTABLES}" "${test_name}"
    CACHE INTERNAL "${TEST_EXECUTABLES}")

    endforeach()

  else()
    set_comp_vars(NAME ${COMPONENT_LIB})

    if("${__SRCS}" STREQUAL "")
      message("INTERFACE LIB: ${COMPONENT_LIB}: srcs: >>>${__SRCS}<<<")
      set(COMP_ACC INTERFACE)
      add_library(${COMPONENT_LIB} INTERFACE ${__SRCS})
    else()
      message("PUBLIC LIB: ${COMPONENT_LIB}: srcs: >>>${__SRCS}<<<")
      set(COMP_ACC PUBLIC)
      add_library(${COMPONENT_LIB} STATIC ${__SRCS})
    endif()

    set(COMPONENT_LIBS
        "${COMPONENT_LIBS}" "${COMPONENT_LIB}"
        CACHE INTERNAL "${COMPONENT_LIBS}")

    set("${COMPONENT_LIB}_ACCESS"
        ${COMP_ACC}
        CACHE INTERNAL "${COMPONENT_LIB}_ACCESS")

    foreach(comp_dir ${COMPONENT_DIRECTORIES})
      foreach(req ${__REQUIRES} ${__PRIV_REQUIRES})
        if(EXISTS "${PROJECT_SOURCE_DIR}/${comp_dir}/${req}/CMakeLists.txt")
          if(NOT EXISTS "${PROJECT_BINARY_DIR}/${comp_dir}/${req}")
            add_subdirectory("${PROJECT_SOURCE_DIR}/${comp_dir}/${req}" "${PROJECT_BINARY_DIR}/${comp_dir}/${req}")
          endif()
        endif()
      endforeach()
    endforeach()

    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt")
      message("test_directory: ${CMAKE_CURRENT_LIST_DIR}/test srcs: ${__SRCS}")
      #if("fernotron_app" STREQUAL ${COMPONENT_LIB}) # XXX: experimental code
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
      #endif()
    endif()
  endif()
endmacro()

function(component_compile_features)
if(NOT "${__SRCS}" STREQUAL "")
  if(NOT ${COMPONENT_LIB} STREQUAL "test")
    target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
  endif()
endif()
endfunction()

function(component_compile_options)
  if(NOT "${__SRCS}" STREQUAL "")
  if(NOT ${COMPONENT_LIB} STREQUAL "test")
    target_compile_options(${COMPONENT_LIB} PRIVATE ${ARGV})
  endif()
  endif()
endfunction()

add_subdirectory(comp/test/test_host)

foreach(comp ${COMPONENT_LIBS})
   message("@all_lib_inc: COMP: ${comp}, ACCESS: ${${comp}_ACCESS}, REQUIRES:${${comp}_REQUIRES}, PRIV_REQUIRES: ${${comp}_PRIV_REQUIRES}")
  my_include_from_list(COMP    ${comp}    ACCESS    ${${comp}_ACCESS}    INC_LIST    ${${comp}_INCLUDE_DIRS}    ${${comp}_PRIV_INCLUDE_DIRS})
  my_comp_add_includes(COMP ${comp} ACCESS ${${comp}_ACCESS} COMP_LIST ${${comp}_REQUIRES})
  my_comp_add_includes(COMP ${comp} ACCESS ${${comp}_ACCESS} COMP_LIST ${${comp}_PRIV_REQUIRES})


  set(reqs ${${comp}_REQUIRES}  ${${comp}_PRIV_REQUIRES})
  foreach(req ${reqs})
    list(FIND COMPONENT_LIBS "${req}" idx)
    if(${idx} EQUAL -1)
      list(REMOVE_ITEM reqs ${req})
    endif()
  endforeach()

  target_link_libraries("${comp}" "${${comp}_ACCESS}" ${reqs})
endforeach()

foreach(comp ${TEST_EXECUTABLES})
   message("@all_test_inc: COMP: ${comp}, ACCESS: ${${comp}_ACCESS}, REQUIRES:${${comp}_REQUIRES}, PRIV_REQUIRES: ${${comp}_PRIV_REQUIRES}")
  my_include_from_list(COMP ${comp} ACCESS PUBLIC INC_LIST    ${${comp}_INCLUDE_DIRS}    ${${comp}_PRIV_INCLUDE_DIRS})
  my_comp_add_includes(COMP ${comp} ACCESS PUBLIC COMP_LIST ${${comp}_REQUIRES})
  my_comp_add_includes(COMP ${comp} ACCESS PUBLIC COMP_LIST ${${comp}_PRIV_REQUIRES})

  set(reqs ${${comp}_REQUIRES}  ${${comp}_PRIV_REQUIRES})
  foreach(req ${reqs})
    list(FIND COMPONENT_LIBS "${req}" idx)
    if(${idx} EQUAL -1)
      list(REMOVE_ITEM reqs ${req})
    endif()
  endforeach()
  target_link_libraries("${comp}" PUBLIC ${reqs})
endforeach()

message("all_libs: ${COMPONENT_LIBS}")

message("all_tests: ${TEST_EXECUTABLES}")
