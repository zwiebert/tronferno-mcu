set(srcs main_setup.cc esp32/main.cc esp32/main_setup.cc esp32/main_loop.cc esp32/main_loop_periodic.cc esp32/unity_setup.cc) 

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES uout firmware_update utils_time utils_debug main_loop
    PRIV_REQUIRES cc1101_ook fernotron_app gpio net utils_misc app_http_server  app_cli app_settings key_value_store net app_mqtt storage config_kvs app_uout app_misc net_http_server
                  freertos
                  
)

component_compile_options(${comp_compile_opts} -lfreertos)
component_compile_features(${comp_compile_feats})