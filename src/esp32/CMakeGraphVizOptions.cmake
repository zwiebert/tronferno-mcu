set(GRAPHVIZ_UNKNOWN_LIBS FALSE)
set(GRAPHVIZ_IGNORE_TARGETS "^[^_][^_][^i][^d]"
     #__idf_app_config __idf_debug _idf_misc$ ^__idf_txtio$ # avoid cluttering
     __idf_esp
     __idf_hal
     __idf_bootloader __idf_esptool_py  __idf_partition_table  __idf_soc __idf_log __idf_spi_flash __idf_micro-ecc __idf_efuse __idf_driver __idf_esp_rom
     __idf_esp_https_ota __idf_app_update __idf_nvs_flash __idf_esp_netif __idf_esp_wifi __idf_esp_eth __idf_esp_http_client __idf_mbedtls __idf_mqtt __idf_lwip
     __idf_spiffs __idf_jsmn
     __idf_esp32 __idf_newlib __idf_heap __idf_cxx __idf_vfs __idf_esp_timer __idf_freertos __idf_xtensa __idf_perfmon __idf_espcoredump
     __idf_app_trace __idf_pthread mbedtls __idf_tcpip_adapter __idf_tcp_transport __idf_nghttp __idf_wpa_supplicant mbedx509 mbedcrypto
)
set(GRAPHVIZ_EXTERNAL_LIBS FALSE)
set(GRAPHVIZ_OBJECT_LIBS FALSE)

