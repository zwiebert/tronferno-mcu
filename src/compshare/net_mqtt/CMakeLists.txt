set(srcs  mqtt.c esp32/mqtt.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/net/mqtt" 
    REQUIRES app_config
    PRIV_REQUIRES cli config userio_app userio
                  mqtt lwip
 )