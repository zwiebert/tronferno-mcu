set(srcs esp32/http_server_content.cc)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
    REQUIRES ""
    PRIV_REQUIRES net net_http_server config cli_app uout_app misc webapp
                  mbedtls  #PRIV_ESP_IDF
   )
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})