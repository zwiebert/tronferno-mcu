set(srcs main_setup.cc esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc esp32/unity_setup.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS "./include/app" "../../esp32" 
    REQUIRES userio firmware_update misc_time debug
    PRIV_REQUIRES fernotron_app gpio net misc http_server_content  cli_app config
                  key_value_store net net_mqtt_app storage config_kvs
)

component_compile_options(${component_compile_opts})