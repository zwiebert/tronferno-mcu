set(srcs dummy.cc)
set(platform_srcs esp32/pin.cc)
include("${CMAKE_CURRENT_LIST_DIR}/../../source_filter.cmake")

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_REQUIRES   utils_debug uout
                  esp_driver_gpio esp_wifi #PRIV_ESP_IDF
)

component_compile_options(${comp_compile_opts})
component_compile_features(${comp_compile_feats})
