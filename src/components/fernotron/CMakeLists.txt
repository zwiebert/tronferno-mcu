set(srcs astro.c  fer_api.c  fer_msg_basic.c  fer_msg_rx.c  fer_msg_tx.c  fer_msg_tx_queue.c fer_radio_rx.c fer_radio_tx.c fer_radio_parity.c fer_rawmsg_buffer.c  fer_rawmsg_build.c  gmbitmask_kvs.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32 -save-temps=obj")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    REQUIRES misc 
    PRIV_REQUIRES app_config app debug key_value_store)

