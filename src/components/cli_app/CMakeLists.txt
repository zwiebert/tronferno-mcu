set(srcs cli.c  cli_out.c
    parm_help.c
    parm_alias.c
    parm_auto.c
    parm_cmd.c
    parm_config.c
    parm_mcu.c
    parm_shpref.c
)

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
    PRIV_INCLUDE_DIRS "./include/cli_app" 
    REQUIRES cli fernotron userio_app 
    PRIV_REQUIRES net net_mqtt_app app misc userio txtio fernotron_alias fernotron_pos debug
)