set(srcs config.cc config_kvs.cc config_comps.cc)



idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron app txtio net_mqtt config_kvs
    PRIV_REQUIRES key_value_store
    )

component_compile_options(-${component_compile_opts})