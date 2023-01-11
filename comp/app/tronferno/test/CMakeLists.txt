idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../app_uout/include ../app_settings/include ../app_mqtt/include ../app_http_server/include ../app_cli/include app_misc/include
                     //  REQUIRES fernotron_app  txtio
                       PRIV_REQUIRES tronferno  uout key_value_store unity  utils_misc app_config config_kvs key_value_store cli
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})






