idf_component_register(
    INCLUDE_DIRS . "./include"
    REQUIRES "misc"
)
   

 add_custom_command(
     OUTPUT #"${COMPONENT_DIR}/include/app_config/proj_kconfig.h"
             ${COMPONENT_DIR}/_force.h   
     COMMAND make all -C ${COMPONENT_DIR}
     VERBATIM)
  
add_custom_target(app_config_kconfig_header ALL  DEPENDS ${COMPONENT_DIR}/_force.h )
add_dependencies(  ${COMPONENT_LIB} app_config_kconfig_header)