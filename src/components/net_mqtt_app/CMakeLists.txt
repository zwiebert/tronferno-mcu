set(srcs  mqtt.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron net_mqtt
    PRIV_REQUIRES app cli userio misc http_server_content
                  mqtt
 )
 
 component_compile_options(-${component_compile_opts})