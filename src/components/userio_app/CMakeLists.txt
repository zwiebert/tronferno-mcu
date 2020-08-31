set(srcs status_output.cc status_output_switch.cc so_out.cc so_print.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES userio 
    PRIV_REQUIRES fernotron app fernotron_pos net_mqtt_app misc)
    
component_compile_options(-${component_compile_opts})