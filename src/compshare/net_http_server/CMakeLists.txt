set(srcs http_server.c  esp32/http_server.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/net/http/server" 
    REQUIRES  app_config
    PRIV_REQUIRES cli_app cli config debug http_server_content userio
                   esp_http_server
 )