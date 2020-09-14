idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../ ../include/app_settings
                       REQUIRES fernotron_app app_misc txtio
                       PRIV_REQUIRES key_value_store unity app_settings
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})