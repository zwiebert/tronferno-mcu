set(srcs esp32/http_server_content.c)
set(content_files "./webapp/build/wapp.js.gz.c"  "./webapp/build/wapp.css.gz.c" "./webapp/build/wapp.html.gz.c" "./webapp/build/wapp.js.map.br.c")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMCU_ESP32")

if(DEFINED ENV{DISTRO})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISTRIBUTION")
endif()


idf_component_register(
    SRCS ${srcs} ${content_files}
    INCLUDE_DIRS . "./include"
    REQUIRES ""
    PRIV_REQUIRES net net_http_server config cli_app userio userio_app misc
   )
   


   add_custom_command(
     OUTPUT ${content_files}
     COMMAND make all -C ${COMPONENT_DIR}/webapp
    # DEPENDS ${COMPONENT_DIR}/logo.bmp
     VERBATIM)

add_custom_target(
  content_files DEPENDS ${content_files}
)

add_dependencies(
  ${COMPONENT_LIB} content_files
)

#set_property(
#  DIRECTORY "${COMPONENT_DIR}/webapp/build"
#  APPEND PROPERTY
#  ADDITIONAL_MAKE_CLEAN_FILES wapp)

   