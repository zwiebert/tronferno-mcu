set(COMPONENT_SRCDIRS ${COMPONENT_SRCDIRS} "../gpio" "../app_update" "../../main" "../../config" "../../debug" "../../net")

set(COMPONENT_EMBED_TXTFILES "../app_update/ca_cert.pem"  "../../net/content/tfmcu.html" "../../net/content/tfmcu.js")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()

register_component(PRIV_REQUIRES fernotron misc)

