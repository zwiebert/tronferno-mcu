set(srcs esp32/http_server_content.cc)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
    REQUIRES ""
    PRIV_REQUIRES net net_http_server config cli_app userio userio_app misc webapp
   )
component_compile_options(-${component_compile_opts})