idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../include/fernotron
                       PRIV_REQUIRES app_settings debug
                       REQUIRES app_misc unity txtio)

                   
if(BUILD_TESTING)
                    
else()
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
endif()