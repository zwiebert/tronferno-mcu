if(NOT UNIT_TESTING) 

set(build_dir ${CMAKE_BINARY_DIR}/app_webapp)

set(web     "wapp.html"
            "wapp.js"
            "wapp.css"
            "wapp.js.map"
 #          "wapp.css.map"
)

set(web_gz ${web})
list(TRANSFORM web_gz APPEND .gz)

set(web_br ${web})
list(TRANSFORM web_br APPEND .br)

set(bins ${web_gz} ${web_br})
list(TRANSFORM bins PREPEND "${build_dir}/")

set(srcs ${bins})
list(TRANSFORM srcs APPEND .c)

 idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "./include"
    REQUIRES net_http_server
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})

add_custom_command(
     OUTPUT ${srcs} ${COMPONENT_DIR}/_force.h   
     COMMAND make BUILD_DIR=${build_dir} all -C ${COMPONENT_DIR} 
     VERBATIM)

add_custom_target(content_files ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} content_files)



else()

idf_component_register(INCLUDE_DIRS "./include")

endif()



