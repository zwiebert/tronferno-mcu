set(srcs commands.cc shutter_pct.cc shutter_prefs_kvs.cc move.cc move_start.cc move_stop.cc move_buf.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron misc_time
    PRIV_REQUIRES app cli userio misc  key_value_store debug config userio_app fernotron_alias fernotron_auto cli_app misc_time
)

component_compile_options(${component_compile_opts})