set(srcs main_setup.cc esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc esp32/unity_setup.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS "./include/app"
    REQUIRES uout firmware_update misc_time dbg
    PRIV_REQUIRES fernotron_app_interface gpio net misc app_http_server  app_cli app_settings key_value_store net app_mqtt storage config_kvs app_uout app_misc
                  freertos
                  
)

component_compile_options(${comp_compile_opts} -lfreertos)
component_compile_features(${comp_compile_feats})