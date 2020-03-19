# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "tronferno-mcu")

set(EXTRA_COMPONENT_DIRS ../components)

set(COMPONENTS http_server_content debug gpio key_value_store storage cli_app config net fernotron_pos fernotron_alias fernotron_cuas fernotron_sep fernotron_auto app userio_app fernotron_txtio txtio userio cli misc fernotron 
	bootloader esptool_py partition_table soc bootloader_support log spi_flash micro-ecc main efuse app_update driver esp_http_server esp_http_client esp_https_ota esp_netif esp_rom esp_wifi mqtt spiffs esp_eth)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(tronferno-mcu esptool_py)
