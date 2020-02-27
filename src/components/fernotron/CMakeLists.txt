#set(COMPONENT_SRCDIRS ${COMPONENT_SRCDIRS} ".") 

set(srcs "fer_api.c"  "fer_msg_basic.c"  "fer_prg.c" "astro.c" "fer_rx_msg.c"  "fer_send_msg.c"  "pins.c"  "transceiver.c")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

#idf_component_register(SRCS ${srcs} INCLUDE_DIRS "../.." "./include" "../../esp32/main")
idf_component_register(SRCS ${srcs} INCLUDE_DIRS "./include" PRIV_INCLUDE_DIRS "./include/fernotron" REQUIRES misc PRIV_REQUIRES app)