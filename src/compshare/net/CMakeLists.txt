set(srcs  ipnet.c 
esp32/ethernet.c  esp32/ipnet.c  esp32/ntp.c  esp32/tcp_server.c  esp32/wifi_ap.c  esp32/wifistation.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()
if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()


idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/net" 
    REQUIRES ""
    PRIV_REQUIRES app app_config config misc net_http_server
                  esp_netif esp_wifi esp_eth
 )