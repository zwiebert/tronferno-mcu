set(srcs cli.cc  cli_out.cc
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
    REQUIRES cli fernotron_app userio_app 
    PRIV_REQUIRES net net_mqtt_app app_misc misc userio txtio fernotron
                  debug http_server_content config key_value_store firmware_update
)

component_compile_options(${component_compile_opts})
