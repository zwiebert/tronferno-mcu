idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "." ../include 
                       PRIV_INCLUDE_DIRS ../ ../include/app/settings
                       REQUIRES fernotron_app_interface app_misc txtio
                       PRIV_REQUIRES key_value_store unity app_settings
)

if(BUILD_TESTING)
  add_executable(test_config_keys test_config_keys.cpp)
  add_test(NAME test_config_keys
           COMMAND testDriver --config $<CONFIGURATION>
                            --exe $<TARGET_FILE:test_config_keys>)                   
else()
component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
endif()





