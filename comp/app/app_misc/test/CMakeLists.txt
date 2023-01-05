idf_component_register(SRC_DIRS "."
                       REQUIRES app_misc app_uout app_config
                                unity)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})                            