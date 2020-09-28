idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../include/fernotron
                       PRIV_REQUIRES fernotron_trx fernotron_app_interface app_config app_settings utils_debug utils_misc txtio
                                     unity
                       #REQUIRES 
                       #app_misc unity txtio  fernotron_trx fernotron_app
                       #fernotron_app_interface app_settings utils_debug config_kvs
                       )


component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
