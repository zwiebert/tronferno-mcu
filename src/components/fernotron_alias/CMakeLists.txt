set(srcs pairings.c pairings_kvs.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} INCLUDE_DIRS . "./include" PRIV_INCLUDE_DIRS "./include/fernotron_alias" REQUIRES fernotron fernotron_pos PRIV_REQUIRES app userio_app txtio)