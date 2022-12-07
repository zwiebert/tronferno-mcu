set(srcs cc1101.cc)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
  #  REQUIRES utils_misc utils_debug
    PRIV_REQUIRES app_config  utils_misc freertos 
                  driver #PRIV_ESP_IDF
     )

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})