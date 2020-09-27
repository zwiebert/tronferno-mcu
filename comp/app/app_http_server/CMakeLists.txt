set(srcs esp32/uri_handlers.cc)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES ""
    PRIV_REQUIRES net net_http_server app_settings app_cli app_uout utils_misc app_webapp
                  mbedtls  #PRIV_ESP_IDF
   )
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})