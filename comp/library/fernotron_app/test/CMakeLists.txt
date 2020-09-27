idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../include/fernotron
                       PRIV_REQUIRES fernotron_interface fernotron_app_interface app_config app_settings dbg misc txtio
                                     unity
                       #REQUIRES 
                       #app_misc unity txtio  fernotron fernotron_app
                       #fernotron_app_interface app_settings dbg config_kvs
                       )


component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
