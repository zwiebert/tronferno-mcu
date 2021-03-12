set(srcs  mqtt.cc mqtt_setup.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES fernotron_app net_mqtt 
    PRIV_REQUIRES app_misc cli utils_misc app_http_server app_uout gpio
 )
 
 component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})