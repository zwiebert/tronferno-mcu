set(srcs set_endpos.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron 
    PRIV_REQUIRES app app_config userio_app txtio gpio
)

component_compile_options(${component_compile_opts})