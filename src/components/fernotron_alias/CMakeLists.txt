set(srcs pairings.cc pairings_kvs.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron 
    PRIV_REQUIRES fernotron_auto app app_config misc userio_app key_value_store debug
)

component_compile_options(${component_compile_opts})