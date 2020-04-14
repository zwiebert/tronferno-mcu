set(srcs esp32/http_server_content.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
    PRIV_INCLUDE_DIRS "./include/http_server_content"
    REQUIRES ""
    PRIV_REQUIRES net net_http_server config cli_app userio userio_app misc
    EMBED_TXTFILES "./webapp/tfmcu.html" "./webapp/tfmcu.js"
   )