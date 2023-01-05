set(srcs  esp32/kvstore.cc rtc.cc  opt_map_consteval.cc) 

if(CONFIG_APP_USE_TIMER_TASK)
     list(APPEND srcs "esp32/timer_task.cc")
else()
     list(APPEND srcs "esp32/timer.cc")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES  utils_time firmware_update
    PRIV_REQUIRES   app_settings  key_value_store nvs_flash
                    driver #PRIV_ESP_IDF
    #EMBED_TXTFILES ./ca_cert.pem
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})