set(srcs timer_data.c  timer_data_fs.c  timer_state.c timer_state_loop.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/fernotron_auto" 
    REQUIRES fernotron 
    PRIV_REQUIRES app userio_app fernotron_pos storage misc debug
)