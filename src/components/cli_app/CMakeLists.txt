set(srcs cli.cc
    parm_help.cc
    parm_alias.cc
    parm_auto.cc
    parm_cmd.cc
    parm_config.cc parm_config_switch.cc
    parm_mcu.cc
    parm_shpref.cc
)

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES cli fernotron
    PRIV_REQUIRES net net_mqtt_app app_misc misc uout_app txtio fernotron_app  debug http_server_content config_app key_value_store firmware_update
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
