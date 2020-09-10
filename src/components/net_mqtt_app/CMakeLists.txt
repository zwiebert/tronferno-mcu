set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron_app net_mqtt uout_app
    PRIV_REQUIRES app_misc cli uout misc http_server_content gpio
                  mqtt
 )
 
 component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})