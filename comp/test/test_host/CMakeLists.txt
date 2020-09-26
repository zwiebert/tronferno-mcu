set(srcs test_runner/setup.c unity/src/unity.c test_runner/test_runner.c test_runner/mock.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS test_runner
    REQUIRES app_main
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
