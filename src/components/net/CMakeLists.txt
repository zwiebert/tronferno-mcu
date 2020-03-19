set(srcs http_server.c  ipnet.c  mqtt.c
esp32/ethernet.c  esp32/http_server.c esp32/ipnet.c  esp32/mqtt.c  esp32/ntp.c  esp32/tcp_server.c  esp32/wifi_ap.c  esp32/wifistation.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/net" 
    REQUIRES fernotron 
    PRIV_REQUIRES app cli userio misc http_server_content)