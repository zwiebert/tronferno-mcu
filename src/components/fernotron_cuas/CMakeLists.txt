set(srcs cuid_auto_set.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron 
    PRIV_REQUIRES cli config app userio_app txtio
)

component_compile_options(-${component_compile_opts})