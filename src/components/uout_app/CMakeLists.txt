set(srcs  status_output_switch.cc so_out.cc so_print.cc callbacks.cc
    #status_output.cc
    so_write_msg.cc so_write_config.cc cli_out.cc
) 

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES uout
    PRIV_REQUIRES fernotron_app app_misc misc config firmware_update debug
    )
    
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
