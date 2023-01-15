if(NOT UNIT_TESTING) 

set(build_dir ${CMAKE_BINARY_DIR}/app_webapp)

set(srcs    "${build_dir}/wapp.html.gz.c"
            "${build_dir}/wapp.js.gz.c"
            "${build_dir}/wapp.css.gz.c"
            "${build_dir}/wapp.js.map.gz.c" "${COMPONENT_DIR}/build/wapp.js.map.br.c"
 #           "${build_dir}/wapp.css.map.gz.c" "${COMPONENT_DIR}/build/wapp.css.map.br.c"
)

set(bins    "${build_dir}/wapp.html.gz"
            "${build_dir}/wapp.js.gz"
            "${build_dir}/wapp.css.gz"
            "${build_dir}/wapp.js.map.gz"
            "${build_dir}/build/wapp.js.map.br"
 #           "${build_dir}/wapp.css.map.gz.c" "${COMPONENT_DIR}/build/wapp.css.map.br.c"
)

 idf_component_register(
    SRCS ${srcs}
    #EMBED_FILES ${bins}
    INCLUDE_DIRS "./include"
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

add_custom_command(
     OUTPUT ${srcs} ${COMPONENT_DIR}/_force.h   
     COMMAND make BUILD_DIR=${CMAKE_BINARY_DIR}/app_webapp all -C ${COMPONENT_DIR} 
     VERBATIM)

add_custom_target(content_files ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} content_files)



else()

#idf_component_register(INCLUDE_DIRS "./include")

endif()



