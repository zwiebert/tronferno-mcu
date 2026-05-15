# Append matching souces in platform_srcs to srcs

set(re_host "(/|^)host/") 
set(re_esp32 "(/|^)esp32/") 
set(re_stm32 "(/|^)stm32/") 

if (ESP_PLATFORM)
  if (IDF_TARGET STREQUAL "linux")
    set(is_host TRUE)
    set(TARGET_PLATFORM_FILTER ${re_host})
  else()
    set(is_esp32 TRUE)
    set(TARGET_PLATFORM_FILTER ${re_esp32})
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(is_host TRUE)
    set(TARGET_PLATFORM_FILTER ${re_host})
elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^cortex-m")
    set(is_cortex_m)
    set(is_stm32)
    set(TARGET_PLATFORM_FILTER ${re_stm32})
endif()
  
#message("TARGET_PLATFORM_FILTER regex for ${CMAKE_CURRENT_SOURCE_DIR} is: <${TARGET_PLATFORM_FILTER}>")
#message("platform sources are: <${platform_srcs}>")

list(FILTER platform_srcs INCLUDE REGEX "${TARGET_PLATFORM_FILTER}")
list(APPEND srcs ${platform_srcs})

#message("filtered platform sources are: <${platform_srcs}>")
