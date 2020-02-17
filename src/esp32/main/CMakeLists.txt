set(COMPONENT_PRIV_INCLUDEDIRS "../..")
set(COMPONENT_SRCDIRS ${COMPONENT_SRCDIRS} "." "../network" "../persistence" "../gpio" "../app_update"
"../../main" "../../misc" "../../cli" "../../fernotron" "../../automatic" "../../shutter_positions"
"../../motor_setup" "../../config" "../../userio" "../../debug" "../../storage" "../../net")

set(COMPONENT_EMBED_TXTFILES "../app_update/ca_cert.pem"  "../../net/content/tfmcu.html" "../../net/content/tfmcu.js")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

if(DEFINED ENV{FLAVOR_LAN})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFLAVOR_LAN")
endif()

register_component()

