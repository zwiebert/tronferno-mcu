#pragma once
#define MCU_TYPE "esp32"

#include <sdkconfig.h>

// override entries from sdkconfig if necessary

//#undef CONFIG_APP_USE_HTTP
//#undef CONFIG_APP_USE_LAN
//#undef CONFIG_APP_USE_WLAN
//#undef CONFIG_APP_USE_WLAN_AP
//#undef CONFIG_APP_USE_NTP
//#undef CONFIG_APP_USE_JSON
//#undef CONFIG_APP_USE_MQTT
//#undef CONFIG_APP_USE_POSIX_TIME
//#undef CONFIG_APP_USE_SERIAL
//#undef CONFIG_APP_USE_WDG
//#undef CONFIG_APP_USE_FS
//#undef CONFIG_APP_USE_HTTP_CLIENT
//#undef CONFIG_APP_USE_OTA
//#undef CONFIG_APP_USE_STM32OTA
//#undef CONFIG_APP_USE_TCPS
//#undef CONFIG_APP_USE_TCPS_TASK
//#undef CONFIG_APP_USE_EG
//#undef CONFIG_APP_USE_SEP
//#undef CONFIG_APP_USE_WS
//#undef CONFIG_APP_USE_CLI_MUTEX
//#undef CONFIG_APP_USE_CLI_TASK
//#undef CONFIG_APP_USE_TXTIO_MUTEX
//#undef CONFIG_APP_USE_NETWORK
//#undef CONFIG_APP_USE_AP_FALLBACK
//#undef CONFIG_APP_USE_CONFIG_KVS
//#undef CONFIG_APP_USE_ESP_GET_TIME
//#undef CONFIG_APP_USE_FER_TRANSMITTER
//#undef CONFIG_APP_USE_FER_RECEIVER
//#undef CONFIG_APP_USE_PAIRINGS
//#undef CONFIG_APP_USE_GPIO_PINS
//#undef CONFIG_APP_USE_TIMER_DATA_FS
//#undef CONFIG_APP_USE_TIMER_DATA_KVS
//#undef CONFIG_APP_USE_FREERTOS
//#undef CONFIG_APP_USE_MDR_TIME

#ifdef UNIT_TESTING
#undef CONFIG_APP_USE_CLI_TASK
#undef CONFIG_APP_USE_NETWORK
#undef CONFIG_APP_USE_WLAN
#undef CONFIG_APP_USE_LAN
#undef CONFIG_APP_USE_WLAN_AP
#undef CONFIG_APP_USE_AP_FALLBACK
#undef CONFIG_APP_USE_MQTT
#undef CONFIG_APP_USE_WS
#endif

#ifndef FLAVOR_LAN
#undef CONFIG_APP_USE_LAN
#endif

//////////////////////////////////////////////////////

//extern const char *const build_date;
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define NO_SPIFFS
#undef CONFIG_BLOB
#define CONFIG_GPIO_SIZE 40
#undef MDR_TIME

#define CFG_NAMESPACE "Tronferno" // XXX: backward compatibility


#include "esp_attr.h"



