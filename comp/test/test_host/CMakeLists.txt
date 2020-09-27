set(srcs test_runner/setup.c test_runner/test_runner.c test_runner/mock.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS test_runner
    #INCLUDE_DIRS unity/src
    REQUIRES app_main
)

add_subdirectory(unity/src)
target_include_directories(test_host
    PRIVATE 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        "$<INSTALL_INTERFACE:src>"

    PRIVATE "src"
)

target_link_libraries(test_host PRIVATE unity)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
