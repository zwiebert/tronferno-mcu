set(srcs commands.c shutter_pct.c shutter_prefs_kvs.c move.c move_start.c move_stop.c move_buf.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_INCLUDE_DIRS "./include/fernotron_pos" 
    REQUIRES fernotron 
    PRIV_REQUIRES app cli userio misc misc_time key_value_store debug
)

