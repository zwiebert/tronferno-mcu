cmake_minimum_required(VERSION 3.16)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>")

project(tfmcu-test)

add_compile_definitions(HOST TEST_HOST TEST HOST_TESTING)
set(TEST_HOST true)
set(UNIT_TESTING true)

include(CTest)

function(my_include_from_list)
  cmake_parse_arguments(MY "" "ACCESS;TAG;COMP" "INC_LIST" "${ARGN}")
   #message("@inc_from_list: TAG:${MY_TAG}, COMP: ${MY_COMP}, ACCESS: ${MY_ACCESS},  INC_LIST: ${MY_INC_LIST}")
  foreach(inc_dir ${MY_INC_LIST})
     #message("${COMPONENT_LIB}: include: ${inc_dir} ")
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

  #message("@comp_add_inc: curr_depth: ${MY_CURRENT_DEPTH}: comps: ${MY_COMP_LIST} seen: ${MY_SEEN_LIST}")

  math(EXPR MY_CURRENT_DEPTH "${MY_CURRENT_DEPTH} + 1")
  if (${MY_CURRENT_DEPTH} GREATER ${MY_MAX_DEPTH})
     message("error: components nested too deep")
     return()
  endif()
  

  foreach(req ${MY_COMP_LIST})
    my_include_from_list(COMP ${MY_COMP} TAG "(depth: ${MY_CURRENT_DEPTH}, ${req})" ACCESS ${MY_ACCESS} INC_LIST ${${req}_INCLUDE_DIRS})

    list(FIND MY_SEEN_LIST ${req} seen_idx)
    if(${seen_idx} EQUAL -1)
      list(APPEND MY_SEEN_LIST ${req})
      my_comp_add_includes(MAX_DEPTH ${MY_MAX_DEPTH} CURRENT_DEPTH ${MY_CURRENT_DEPTH} SEEN_LIST "${MY_SEEN_LIST}"
                           COMP ${MY_COMP} ACCESS  ${MY_ACCESS} COMP_LIST ${${req}_REQUIRES})
    endif()
  endforeach()

endfunction()


set(test_this 0)
#set(MY_SEEN_LIST a b c)
set(req b)
list(FIND MY_SEEN_LIST "${req}" seen_idx)
message("si: ${seen_idx}")


if(${test_this})
message("start testing this")
set(test_comp_list app_main)

my_comp_add_includes(ACCESS "PUBLIC" COMP_LIST ${test_comp_list})

return()
endif()



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
      add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
    endforeach()
  else()

    set(COMPONENT_LIBS
        "${COMPONENT_LIBS}" "${COMPONENT_LIB}"
        CACHE INTERNAL "${COMPONENT_LIBS}")

    if(DEFINED __SRCS)
      #message("PUBLIC LIB: ${COMPONENT_LIB}: srcs: >>>${__SRCS}<<<")
      set(COMP_ACC PUBLIC)
      add_library(${COMPONENT_LIB} STATIC ${__SRCS})
    else()
      #message("INTERFACE LIB: ${COMPONENT_LIB}: srcs: >>>${__SRCS}<<<")
      set(COMP_ACC INTERFACE)
      add_library(${COMPONENT_LIB} INTERFACE ${__SRCS})
    endif()

    foreach(inc_dir ${FORCE_INCLUDE_LIST})
      target_include_directories(${COMPONENT_LIB} ${COMP_ACC} ${inc_dir})
    endforeach()

    my_include_from_list(COMP ${COMPONENT_LIB} ACCESS ${COMP_ACC} INC_LIST ${__INCLUDE_DIRS})
    my_include_from_list(COMP ${COMPONENT_LIB} ACCESS ${COMP_ACC} INC_LIST ${__PRIV_INCLUDE_DIRS})

    list(TRANSFORM __INCLUDE_DIRS PREPEND "${CMAKE_CURRENT_LIST_DIR}/" OUTPUT_VARIABLE INC_PATHS)
    set("${COMPONENT_LIB}_INCLUDE_DIRS"
        ${INC_PATHS}
        CACHE INTERNAL "${COMPONENT_LIB}_INCLUDE_DIRS")

    list(TRANSFORM _PRIV_INCLUDE_DIRS PREPEND "${CMAKE_CURRENT_LIST_DIR}/" OUTPUT_VARIABLE PRIV_INC_PATHS)
    set("${COMPONENT_LIB}_PRIV_INCLUDE_DIRS"
        ${PRIV_INC_PATHS}
        CACHE INTERNAL "${COMPONENT_LIB}_PRIV_INCLUDE_DIRS")

    set("${COMPONENT_LIB}_REQUIRES"
        ${__REQUIRES}
        CACHE INTERNAL "${COMPONENT_LIB}_REQUIRES")

        set("${COMPONENT_LIB}_PRIV_REQUIRES"
        ${__PRIV_REQUIRES}
        CACHE INTERNAL "${COMPONENT_LIB}_PRIV_REQUIRES")

    set("${COMPONENT_LIB}_ACCESS"
        ${COMP_ACC}
        CACHE INTERNAL "${COMPONENT_LIB}_ACCESS")

    set(CHILD_INC_PATHS
        ${CHILD_INC_PATHS} ${INC_PATHS}
        PARENT_SCOPE)

    foreach(comp_dir comp/app;comp/external/components-mcu/components;comp/library)
      foreach(req ${__REQUIRES} ${__PRIV_REQUIRES})
        if(EXISTS "${PROJECT_SOURCE_DIR}/${comp_dir}/${req}/CMakeLists.txt")
          if(NOT EXISTS "${PROJECT_BINARY_DIR}/${comp_dir}/${req}")
            add_subdirectory("${PROJECT_SOURCE_DIR}/${comp_dir}/${req}" "${PROJECT_BINARY_DIR}/${comp_dir}/${req}")
            my_include_from_list(COMP ${COMPONENT_LIB} ACCESS ${COMP_ACC} INC_LIST ${CHILD_INC_PATHS})
          else()
           # my_comp_add_includes(COMP ${COMPONENT_LIB} ACCESS ${COMP_ACC} COMP_LIST ${req})
          endif()
        endif()
      endforeach()
    endforeach()

    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt")
      message("test_directory: ${CMAKE_CURRENT_LIST_DIR}/test srcs: ${__SRCS}")
      if("fernotron_app" STREQUAL ${COMPONENT_LIB}) # XXX: experimental code
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
      endif()
    endif()
  endif()
endmacro()

function(component_compile_features)
  target_compile_features(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

function(component_compile_options)
  target_compile_options(${COMPONENT_LIB} PRIVATE ${ARGV})
endfunction()

add_subdirectory(comp/app/app_main)


foreach(comp ${COMPONENT_LIBS})
   #message("@all_inc: comp: ${comp} access: ${${comp}_ACCESS} requires:${${comp}_REQUIRES} priv_requires: ${${comp}_PRIV_REQUIRES}")
   my_include_from_list(COMP ${comp} ACCESS ${${comp}_ACCESS} INC_LIST ${${comp}_INCLUDE_DIRS} ${${comp}_PRIV_INCLUDE_DIRS})
   my_comp_add_includes(COMP ${comp} ACCESS ${${comp}_ACCESS}  COMP_LIST ${${comp}_REQUIRES})
   my_comp_add_includes(COMP ${comp} ACCESS ${${comp}_ACCESS}  COMP_LIST ${${comp}_PRIV_REQUIRES})
endforeach()


message("all: ${COMPONENT_LIBS}")