/**
 * \file app_config/host/proj_app_cfg.h
 * \brief override entries from sdkconfig.h if necessary
 */
#define MCU_TYPE "host"

#include "./sdkconfig.h"

// override entries from sdkconfig if necessary

#undef CONFIG_APP_USE_HTTP
#undef CONFIG_APP_USE_LAN
#undef CONFIG_APP_USE_WLAN
#undef CONFIG_APP_USE_WLAN_AP
#undef CONFIG_APP_USE_NTP
#undef CONFIG_APP_USE_JSON
#undef CONFIG_APP_USE_MQTT
//#undef CONFIG_APP_USE_POSIX_TIME
#undef CONFIG_APP_USE_SERIAL
#undef CONFIG_APP_USE_WDG
#undef CONFIG_APP_USE_FS
#undef CONFIG_APP_USE_HTTP_CLIENT
#undef CONFIG_APP_USE_OTA
#undef CONFIG_APP_USE_STM32OTA
#undef CONFIG_APP_USE_TCPS
#undef CONFIG_APP_USE_TCPS_TASK
#undef CONFIG_APP_USE_EG
#undef CONFIG_APP_USE_SEP
#undef CONFIG_APP_USE_WS
#undef CONFIG_APP_USE_MUTEX
#undef CONFIG_APP_USE_CLI_TASK
#undef CONFIG_APP_USE_TXTIO_MUTEX
#undef CONFIG_APP_USE_NETWORK
#undef CONFIG_APP_USE_AP_FALLBACK
#undef CONFIG_APP_USE_CONFIG_KVS
#undef CONFIG_APP_USE_ESP_GET_TIME
//#undef CONFIG_APP_USE_FER_TRANSMITTER
//#undef CONFIG_APP_USE_FER_RECEIVER
#undef CONFIG_APP_USE_PAIRINGS
#undef CONFIG_APP_USE_GPIO_PINS
#undef CONFIG_APP_USE_TIMER_DATA_FS
//#undef CONFIG_APP_USE_TIMER_DATA_KVS
#undef CONFIG_APP_USE_FREERTOS
#undef CONFIG_APP_USE_MDR_TIME
///////////////////////////////////////////////

#define IRAM_ATTR
#undef MDR_TIME

#define NO_SPIFFS

#undef MDR_TIME
#define MY_RFOUT_GPIO -1
#define MY_RFIN_GPIO -1
#define MY_SETBUTTON_GPIO -1
#define ISO_BUILD_TIME (__DATE__ "T" __TIME__)
#define MY_RF_TRX rfTrxNone

