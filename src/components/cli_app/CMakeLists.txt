set(srcs cli.c  cli_config.c  cli_json.c  cli_mcu.c  cli_out.c  cli_parm_pair.c parm_shpref.c cli_send.c  cli_timer.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} INCLUDE_DIRS . "./include" PRIV_INCLUDE_DIRS "./include/cli_app" REQUIRES cli fernotron userio_app PRIV_REQUIRES net app misc userio txtio fernotron_alias fernotron_pos)