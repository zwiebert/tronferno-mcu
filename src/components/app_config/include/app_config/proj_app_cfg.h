// User configuration
#pragma once

#define APP_VERSION "0.10.9"

#define FER_TRANSMITTER
#define FER_RECEIVER
#define USE_NETWORK
#define USE_NTP
#define USE_WLAN
#define USE_WLAN_AP
#define USE_LAN
#define USE_TCPS
#define USE_TCPS_TASK
#define USE_MQTT
#define USE_WS
#define USE_HTTP
#define CONFIG_BLOB
#define CONFIG_DICT
#define USE_CONFIG_KVS
#define ACCESS_GPIO
#define POSIX_TIME 1
#define USE_ESP_GET_TIME
#define MDR_TIME
#define USE_PAIRINGS
#define USE_CUAS
#define USE_JSON
#define USE_CLI_MUTEX
#define USE_CLI_TASK
#define USE_OTA
#define USE_TXTIO_MUTEX
#define USE_TIMER_DATA_FS
#define USE_TIMER_DATA_KVS
#define USE_EG
#define USE_AP_FALLBACK
#define USE_SEP

//#define USE_PCT_ARRAY

#define USE_FREERTOS

#include "callbacks.h"

#include "misc/int_types.h"


#define ENABLE_SET_ENDPOS 1

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3
#endif


#define CHECK_NETWORK_INTERVAL 15
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25

#define WIFI_AP_SSID "tronferno"
#define WIFI_AP_PASSWD "tronferno"


#define TD_FS_TO_KVS


#if defined TEST_HOST
#define IRAM_ATTR

//#undef FER_TRANSMITTER
#undef FER_RECEIVER
#undef USE_NETWORK
#undef USE_NTP
#undef USE_WLAN
#undef USE_WLAN_AP
#undef USE_LAN
#undef USE_TCPS
#undef USE_TCPS_TASK
#undef USE_MQTT
#undef USE_WS
#undef USE_HTTP
#undef CONFIG_BLOB
#undef CONFIG_DICT
//#undef USE_CONFIG_KVS
#undef ACCESS_GPIO
//#undef POSIX_TIME 1
#undef USE_ESP_GET_TIME
#undef MDR_TIME
#undef USE_PAIRINGS
#undef USE_CUAS
#undef USE_JSON
#undef USE_CLI_MUTEX
#undef USE_CLI_TASK
#undef USE_OTA
#undef USE_TXTIO_MUTEX
#undef USE_TIMER_DATA_FS
//#undef USE_TIMER_DATA_KVS
#undef USE_EG
#undef USE_AP_FALLBACK
#undef USE_SEP



#define NO_SPIFFS
#undef USE_FREERTOS

#undef USE_ESP_GET_TIME
#undef MDR_TIME
#define MY_RFOUT_GPIO -1
#define MY_RFIN_GPIO -1
#define MY_SETBUTTON_GPIO -1
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)
#define MCU_TYPE "host"

#elif defined MCU_ESP8266
#include "app_config/esp8266/esp8266_user_config.h"

#elif defined MCU_ESP32
#include "app_config/esp32/esp32_user_config.h"

#else
#error "no supported MCU"
#define IRAM_ATTR
#define DRAM_ATTR
#endif


