#pragma once
#define MCU_TYPE "esp8266"

#include "sdkconfig.h"

extern const char *const build_time;
#define ISO_BUILD_TIME build_time


// override entries from sdkconfig if necessary

#undef CONFIG_TF_USE_HTTP
#undef CONFIG_TF_USE_LAN
//#undef CONFIG_TF_USE_WLAN
#undef CONFIG_TF_USE_WLAN_AP
//#undef CONFIG_TF_USE_NTP
//#undef CONFIG_TF_USE_JSON
#undef CONFIG_TF_USE_MQTT
#undef CONFIG_TF_USE_POSIX_TIME
//#undef CONFIG_TF_USE_SERIAL
//#undef CONFIG_TF_USE_WDG
//#undef CONFIG_TF_USE_FS
//#undef CONFIG_TF_USE_HTTP_CLIENT
#undef CONFIG_TF_USE_OTA
//#undef CONFIG_TF_USE_STM32OTA
//#undef CONFIG_TF_USE_TCPS
#undef CONFIG_TF_USE_TCPS_TASK
//#undef CONFIG_TF_USE_EG
//#undef CONFIG_TF_USE_SEP
#undef CONFIG_TF_USE_WS
#undef CONFIG_TF_USE_CLI_MUTEX
#undef CONFIG_TF_USE_CLI_TASK
#undef CONFIG_TF_USE_TXTIO_MUTEX
//#undef CONFIG_TF_USE_NETWORK
//#undef CONFIG_TF_USE_AP_FALLBACK
//#undef CONFIG_TF_USE_CONFIG_KVS
#undef CONFIG_TF_USE_ESP_GET_TIME
//#undef CONFIG_TF_USE_FER_TRANSMITTER
//#undef CONFIG_TF_USE_FER_RECEIVER
//#undef CONFIG_TF_USE_PAIRINGS
//#undef CONFIG_TF_USE_GPIO_PINS
//#undef CONFIG_TF_USE_TIMER_DATA_FS
#undef CONFIG_TF_USE_TIMER_DATA_KVS
#undef CONFIG_TF_USE_FREERTOS
#define CONFIG_TF_USE_MDR_TIME



#define CONFIG_GPIO_SIZE 17
#undef TD_FS_TO_KVS

#include "user_interface.h"
#define IRAM_ATTR  __attribute__((section(".iram0.text")))

#include <osapi.h>

#define snprintf ets_snprintf
#define sprintf ets_sprintf
#ifdef __cplusplus
extern "C" void  user_pre_init(void);
extern "C" void  user_init(void);
#endif

#define NO_CXX_MOVE

