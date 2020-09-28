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
    INCLUDE_DIRS "./include" 
    REQUIRES cli
    PRIV_REQUIRES net net_http_server app_mqtt app_misc utils_misc app_uout txtio fernotron_app fernotron_trx utils_debug app_http_server app_settings key_value_store firmware_update
    EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
