/**
 * \file  app_config/proj_app_cfg.h
 * \brief application configuration
 */

#pragma once

#define CONFIG_BLOB
#define CONFIG_DICT

#define MDR_TIME

//#define USE_PCT_ARRAY

#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;


//#include "utils_misc/int_types.h"

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
#include "app_config/host/proj_app_cfg.h"

#elif defined MCU_ESP8266
#include "app_config/esp8266/proj_app_cfg.h"

#elif defined MCU_ESP32
#include "app_config/esp32/proj_app_cfg.h"

#else
#include "app_config/none/proj_app_cfg.h"
#error "no supported MCU"
#define MCU_ESP32
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
#ifdef USE_MDR_TIME
#define MDR_TIME
#endif

#define MY_MQTT_CLIENT_ID "tfmcu"
#define MY_MQTT_ROOT_TOPIC "tfmcu"

#ifdef __cplusplus
#include <utils_misc/cstring_utils.hh>
#define STRLCPY(dst,src,size) csu_copy((dst),(size),(src))
#define STRCPY(dst,src) csu_copy((dst),(src))
#else
#define STRLCPY(dst,src,size) strlcpy((dst),(src),(size))
#define STRCPY(dst,src) strcpy((dst),(src))
#endif

