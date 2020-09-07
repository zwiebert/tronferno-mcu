set(srcs status_output.cc status_output_switch.cc so_out.cc so_print.cc) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES userio
    PRIV_REQUIRES fernotron_app app net_mqtt_app misc cli_app config firmware_update)
    
component_compile_options(${component_compile_opts})