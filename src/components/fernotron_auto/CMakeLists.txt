set(srcs timer_data.cc  timer_data_fs.cc timer_data_kvs.cc timer_minutes.cc timer_event.cc) 

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    REQUIRES fernotron 
    PRIV_REQUIRES app userio_app fernotron_pos storage misc debug cli_app config key_value_store
)

component_compile_options(${component_compile_opts})