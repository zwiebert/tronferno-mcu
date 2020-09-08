// User configuration
#pragma once

#define APP_VERSION "0.10.9"


#define CONFIG_BLOB
#define CONFIG_DICT

#define MDR_TIME

//#define USE_PCT_ARRAY

#include "misc/int_types.h"

#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3
#endif
#define CHECK_NETWORK_INTERVAL 15
#define LOOP_PERIODIC_INTERVAL_MS 100
#define LOOP_INTERVAL_MS 25
#define WIFI_AP_SSID "tronferno"
#define WIFI_AP_PASSWD "tronferno"
#define TD_FS_TO_KVS

// XXX: to show "all" code ifdefs
#ifndef HOST_TESTING
#undef TEST_HOST
#if !defined MCU_ESP32 && !defined MCU_ESP8266
#define MCU_ESP32
#endif
#endif

#if defined TEST_HOST
#include "app_config/host/proj_app_cfg.h"

#elif defined MCU_ESP8266
#include "app_config/esp8266/proj_app_cfg.h"

#elif defined MCU_ESP32 || defined ESP_PLATFORM
#include "app_config/esp32/proj_app_cfg.h"

#else
#error "no supported MCU"

#endif

#include "proj_kconfig.h"

#ifdef USE_FER_RECEIVER
#define FER_RECEIVER
#endif
#ifdef USE_FER_TRANSMITTER
#define FER_TRANSMITTER
#endif
#ifdef USE_POSIX_TIME
#define POSIX_TIME 1
#endif
#ifdef USE_SEP
#define ENABLE_SET_ENDPOS 1
#endif
#ifdef USE_GPIO_PINS
#define ACCESS_GPIO
#endif
#ifdef USE_MDR_TIME
#define MDR_TIME
#endif

