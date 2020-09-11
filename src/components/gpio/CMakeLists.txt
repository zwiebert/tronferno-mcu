set(srcs esp32/gpinput.cc esp32/pin.cc)

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS . "./include" 
    PRIV_REQUIRES txtio config app_config app_misc debug uout_app
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})