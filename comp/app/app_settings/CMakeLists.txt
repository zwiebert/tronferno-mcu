set(srcs config.cc config_kvs.cc config_comps.cc)



idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron_app app_misc txtio net_mqtt config_kvs gpio
    PRIV_REQUIRES key_value_store app_mqtt app_cli fernotron
    )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
