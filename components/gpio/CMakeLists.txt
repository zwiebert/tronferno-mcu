set(srcs dummy.cc)
set(platform_srcs esp32/pin.cc)
include("${CMAKE_CURRENT_LIST_DIR}/../../source_filter.cmake")

idf_component_register(SRCS ${srcs} 
    INCLUDE_DIRS "./include" 
    PRIV_REQUIRES   utils_debug uout
                  esp_driver_gpio esp_wifi #PRIV_ESP_IDF
)

 
target_compile_options(${COMPONENT_LIB} PRIVATE "SHELL:-include sdkconfig.h" -Wno-error=missing-field-initializers)
