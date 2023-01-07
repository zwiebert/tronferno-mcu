idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                     //  REQUIRES fernotron_app app_misc txtio
                       PRIV_REQUIRES app_cli uout key_value_store unity app_settings app_uout utils_misc
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})






