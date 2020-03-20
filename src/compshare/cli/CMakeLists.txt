set(srcs cli.c 
 cmdline_get.c  cmdline_parse_json.c  cmdline_parse.c  cmdline_set.c cmdline_process.c
 esp32/mutex.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include"  
    PRIV_INCLUDE_DIRS "./include/cli"
    REQUIRES userio 
    PRIV_REQUIRES app_config config debug misc txtio userio
)