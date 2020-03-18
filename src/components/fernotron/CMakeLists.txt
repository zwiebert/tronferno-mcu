set(srcs astro.c  fer_api.c  fer_msg_basic.c  fer_msg_rx.c  fer_msg_tx.c  fer_radio_trx.c  fer_rawmsg_buffer.c  fer_rawmsg_build.c  gmbitmask_kvs.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(SRCS ${srcs} INCLUDE_DIRS "./include" PRIV_INCLUDE_DIRS "./include/fernotron" REQUIRES misc PRIV_REQUIRES app debug key_value_store)

