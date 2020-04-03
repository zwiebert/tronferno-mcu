set(srcs fernotron.c esp32/timer.c esp32/ota.c esp32/main.c esp32/main_setup.c esp32/main_loop.c rtc.c esp32/unity_setup.c) 

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()
if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include" ../.. 
    PRIV_INCLUDE_DIRS "./include/app" "../../esp32" 
    REQUIRES userio main 
    PRIV_REQUIRES fernotron gpio net
                  esp_https_ota
    EMBED_TXTFILES ./esp32/ca_cert.pem
)