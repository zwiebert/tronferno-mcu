set(srcs commands.c shutter_pct.c shutter_pct_kvs.c shutter_prefs_kvs.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} INCLUDE_DIRS . "./include" PRIV_INCLUDE_DIRS "./include/fernotron_pos" REQUIRES fernotron PRIV_REQUIRES app cli userio misc key_value_store)