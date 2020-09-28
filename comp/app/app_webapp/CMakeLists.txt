if(NOT UNIT_TESTING) 

set(srcs    "${COMPONENT_DIR}/build/wapp.html.gz.c"
            "${COMPONENT_DIR}/build/wapp.js.gz.c"
            "${COMPONENT_DIR}/build/wapp.css.gz.c"
            "${COMPONENT_DIR}/build/wapp.js.map.gz.c" "${COMPONENT_DIR}/build/wapp.js.map.br.c"
            "${COMPONENT_DIR}/build/wapp.css.map.gz.c" "${COMPONENT_DIR}/build/wapp.css.map.br.c"
)
            
 idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
 
add_custom_command(
     OUTPUT ${srcs} ${COMPONENT_DIR}/_force.h   
     COMMAND make all -C ${COMPONENT_DIR}
     VERBATIM)
  
add_custom_target(content_files ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} content_files)



else()

#idf_component_register(INCLUDE_DIRS "./include")

endif()



