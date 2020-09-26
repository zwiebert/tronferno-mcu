set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES fernotron_app_interface net_mqtt 
    PRIV_REQUIRES app_misc cli misc app_http_server app_uout
 )
 
 component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})