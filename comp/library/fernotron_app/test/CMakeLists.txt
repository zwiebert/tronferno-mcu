idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../include/fernotron
                       PRIV_REQUIRES fernotron_trx fernotron_app fernotron_app  utils_debug utils_misc txtio utils_time 
                                     unity
                       )


component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
