set(srcs commands.cc shutter_pct.cc shutter_prefs_kvs.cc move.cc move_start.cc move_stop.cc move_buf.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron 
    PRIV_REQUIRES app cli userio misc misc_time key_value_store debug config userio_app fernotron_alias fernotron_auto cli_app
)

component_compile_options(${component_compile_opts})