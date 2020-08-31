set(srcs fer_print.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron 
    PRIV_REQUIRES fernotron txtio misc
)
component_compile_options(-${component_compile_opts})