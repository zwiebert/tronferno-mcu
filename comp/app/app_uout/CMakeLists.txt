set(srcs so_print.cc #callbacks.cc
    so_write_msg.cc so_write_config.cc
) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES uout fernotron_uout
    PRIV_REQUIRES  app_misc utils_misc app_settings  utils_debug fernotron_app  cli net cc1101_ook
                   app_update
    )


component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
