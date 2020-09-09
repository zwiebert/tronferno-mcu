idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../ ../include/app
                       REQUIRES app_misc unity txtio)


component_compile_options(${component_compile_opts})                            