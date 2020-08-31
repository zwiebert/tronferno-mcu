set(srcs esp32/gpinput.cc esp32/pin.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_REQUIRES txtio config app_config app
)

component_compile_options(-${component_compile_opts})