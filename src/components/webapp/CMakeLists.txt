set(srcs
"../webapp/build/wapp.html.gz.c"
"../webapp/build/wapp.js.gz.c"
"../webapp/build/wapp.css.gz.c"
"../webapp/build/wapp.js.map.br.c"
)



set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS . "./include"
   )
 
 add_custom_command(
     OUTPUT "${COMPONENT_DIR}/build/wapp.html.gz.c"
            "${COMPONENT_DIR}/build/wapp.js.gz.c"
            "${COMPONENT_DIR}/build/wapp.css.gz.c"
            "${COMPONENT_DIR}/build/wapp.js.map.br.c"
             ${COMPONENT_DIR}/_force.h   
     COMMAND make all -C ${COMPONENT_DIR}
     VERBATIM)
  
add_custom_target(content_files ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} content_files)