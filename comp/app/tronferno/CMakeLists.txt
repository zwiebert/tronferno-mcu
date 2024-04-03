set(srcs_all)
set(pub_req_all)
set(priv_req_all)
set(embed_txtfiles_all)

set(srcs
    app_cli/cli.cc
    app_cli/parm_help.cc
    app_cli/parm_alias.cc
    app_cli/parm_auto.cc
    app_cli/parm_cmd.cc
    app_cli/parm_config.cc app_cli/parm_config_comp.cc app_cli/parm_config_app.cc app_cli/parm_config_help.cc
    app_cli/parm_mcu.cc
    app_cli/parm_sep.cc
    app_cli/parm_shpref.cc
    app_cli/parm_kvs.cc
    app_cli/cli_to_json.cc


    app_main/main_setup.cc app_main/esp32/main.cc app_main/esp32/main_setup.cc app_main/esp32/main_loop.cc app_main/esp32/main_loop_periodic.cc app_main/esp32/unity_setup.cc

    app_misc/esp32/kvstore.cc app_misc/rtc.cc  app_misc/opt_map_consteval.cc

    app_settings/config.cc app_settings/config_kvs.cc app_settings/config_comps.cc app_settings/app_settings.cc app_settings/json_save.cc

    app_uout/so_print.cc app_uout/so_write_msg.cc app_uout/so_write_config.cc )

if(CONFIG_APP_USE_MQTT)
 list(APPEND srcs app_mqtt/mqtt.cc app_mqtt/mqtt_setup.cc)
 endif()

 if(CONFIG_APP_USE_HTTP)
 list(APPEND srcs  app_http_server/web_content.cc app_http_server/esp32/uri_handlers.cc)
 endif()

if(CONFIG_APP_USE_TIMER_TASK)
 list(APPEND srcs "app_misc/esp32/timer_task.cc")
else()
 list(APPEND srcs "app_misc/esp32/timer.cc")
endif()


idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS  app_uout/include app_settings/include app_mqtt/include app_http_server/include app_cli/include app_misc/include
    REQUIRES  cli config_kvs fernotron_app fernotron_uout firmware_update gpio main_loop net_mqtt_client txtio uout utils_debug utils_time
    PRIV_REQUIRES app_webapp cc1101_ook cli fernotron_app fernotron_trx firmware_update gpio kvs main_loop net net_http_server nvs_flash storage txtio utils_debug utils_misc 
                  esp_wifi driver mbedtls app_update freertos 
    EMBED_TXTFILES app_cli/ca_cert.pem
    )


component_compile_options(${comp_compile_opts} -lfreertos)
component_compile_features(${comp_compile_feats})
