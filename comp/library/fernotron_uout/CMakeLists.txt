set(srcs   fer_uo_publish.cc
    )

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES utils_misc
    PRIV_REQUIRES app_config utils_debug  txtio uout fernotron_trx
     )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
