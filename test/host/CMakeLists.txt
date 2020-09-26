cmake_minimum_required(VERSION 3.16)

set(comp_compile_feats cxx_std_20)
set(comp_compile_opts "$<$<COMPILE_LANGUAGE:CXX>:-Wno-narrowing>" -Wno-missing-field-initializers)
set(comp_compile_opts ${comp_compile_opts} "$<$<COMPILE_LANGUAGE:CXX>:-std=gnu++2a>")

project(tfmcu-test)

add_compile_definitions(HOST TEST_HOST TEST HOST_TESTING)
set(TEST_HOST true)
set(UNIT_TESTING true)

include(CTest)

set(FORCE_INCLUDE_LIST /home/bertw/proj/mcu/tronferno-mcu/test/host/comp/external/components-mcu/components/misc/./include 
/home/bertw/proj/mcu/tronferno-mcu/test/host/comp/external/components-mcu/components/config_kvs/./include
                       /home/bertw/proj/mcu/tronferno-mcu/test/host/comp/library/gpio/./include 
                       /home/bertw/proj/mcu/tronferno-mcu/test/host/comp/library/fernotron/./include
                       /home/bertw/proj/mcu/tronferno-mcu/test/host/comp/library/fernotron_interface/./include
                        /home/bertw/proj/mcu/tronferno-mcu/test/host/comp/library/fernotron_app/./include
                        )

function(my_include_from_list)
  cmake_parse_arguments(MY "" "ACCESS;TAG" "INC_LIST" "${ARGN}")
  # message("~~~~~~${MY_TAG}~~~~~~~: ${COMPONENT_LIB}:${MY_ACCESS}: ${MY_INC_LIST}")
  foreach(inc_dir ${MY_INC_LIST})
    # message("${COMPONENT_LIB}: include: ${inc_dir} ")
    target_include_directories(${COMPONENT_LIB} ${MY_ACCESS} ${inc_dir})
  endforeach()
endfunction()

function(my_comp_add_includes)
  cmake_parse_arguments(MY "" "" "COMP_LIST" "${ARGN}")
  foreach(req ${MY_COMP_LIST})
    my_include_from_list(TAG "(((((((${req}))))))" ACCESS ${COMP_ACC} INC_LIST ${${req}_INCLUDE_DIRS})
    foreach(child_req ${${req}_REQUIRES})
      my_include_from_list(TAG "##1###${child_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child_req}_INCLUDE_DIRS})
      foreach(child2_req ${${child_req}_REQUIRES})
        my_include_from_list(TAG "##2##${child2_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child2_req}_INCLUDE_DIRS})
        foreach(child3_req ${${child2_req}_REQUIRES})
          my_include_from_list(TAG "##3##${child3_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child3_req}_INCLUDE_DIRS})
          foreach(child4_req ${${child3_req}_REQUIRES})
            my_include_from_list(TAG "##4##${child4_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child4_req}_INCLUDE_DIRS})
            foreach(child5_req ${${child4_req}_REQUIRES})
              my_include_from_list(TAG "##5##${child5_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child5_req}_INCLUDE_DIRS})
              foreach(child6_req ${${child5_req}_REQUIRES})
                my_include_from_list(TAG "##6##${child6_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child6_req}_INCLUDE_DIRS})
                foreach(child7_req ${${child6_req}_REQUIRES})
                  my_include_from_list(TAG "##7##${child7_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child7_req}_INCLUDE_DIRS})
                  foreach(child8_req ${${child7_req}_REQUIRES})
                    my_include_from_list(TAG "##8##${child8_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child8_req}_INCLUDE_DIRS})
                    foreach(child9_req ${${child8_req}_REQUIRES})
                      my_include_from_list(TAG "##9##${child9_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child9_req}_INCLUDE_DIRS})
                      foreach(child10_req ${${child9_req}_REQUIRES})
                        my_include_from_list(TAG "##10##${child10_req}#######" ACCESS ${COMP_ACC} INC_LIST ${${child11_req}_INCLUDE_DIRS})
                      endforeach()
                    endforeach()
                  endforeach()
                endforeach()
              endforeach()
            endforeach()
          endforeach()
        endforeach()
      endforeach()
    endforeach()
  endforeach()
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
      message("PUBLIC LIB: ${COMPONENT_LIB}: srcs: >>>${__SRCS}<<<")
      set(COMP_ACC PUBLIC)
      add_library(${COMPONENT_LIB} STATIC ${__SRCS})
    else()
      message("INTERFACE LIB: ${COMPONENT_LIB}: srcs: >>>${__SRCS}<<<")
      set(COMP_ACC INTERFACE)
      add_library(${COMPONENT_LIB} INTERFACE ${__SRCS})
    endif()

    foreach(inc_dir ${FORCE_INCLUDE_LIST})
      target_include_directories(${COMPONENT_LIB} ${COMP_ACC} ${inc_dir})
    endforeach()

    my_include_from_list(ACCESS ${COMP_ACC} INC_LIST ${__INCLUDE_DIRS})
    my_include_from_list(ACCESS ${COMP_ACC} INC_LIST ${__PRIV_INCLUDE_DIRS})

    list(TRANSFORM __INCLUDE_DIRS PREPEND "${CMAKE_CURRENT_LIST_DIR}/" OUTPUT_VARIABLE INC_PATHS)
    message("inc_paths: ${INC_PATHS}  +  ${CHILD_INC_PATHS}")

    set("${COMPONENT_LIB}_INCLUDE_DIRS"
        ${INC_PATHS}
        CACHE INTERNAL "${COMPONENT_LIB}_INCLUDE_DIRS")
    set("${COMPONENT_LIB}_REQUIRES"
        ${__REQUIRES}
        CACHE INTERNAL "${COMPONENT_LIB}_REQUIRES")
    set(CHILD_INC_PATHS
        ${CHILD_INC_PATHS} ${INC_PATHS}
        PARENT_SCOPE)

    foreach(comp_dir comp/app;comp/external/components-mcu/components;comp/library)
      foreach(req ${__REQUIRES} ${__PRIV_REQUIRES})
        if(EXISTS "${PROJECT_SOURCE_DIR}/${comp_dir}/${req}/CMakeLists.txt")
          if(NOT EXISTS "${PROJECT_BINARY_DIR}/${comp_dir}/${req}")
            add_subdirectory("${PROJECT_SOURCE_DIR}/${comp_dir}/${req}" "${PROJECT_BINARY_DIR}/${comp_dir}/${req}")
            my_include_from_list(ACCESS ${COMP_ACC} INC_LIST ${CHILD_INC_PATHS})
          else()
            my_comp_add_includes(COMP_LIST ${req})
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

message("all: ${COMPONENT_LIBS}")
