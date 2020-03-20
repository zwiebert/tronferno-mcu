set(srcs esp32/gpinput.c esp32/pin.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()
if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/gpio"
    PRIV_REQUIRES txtio config app_config app
)