/**
 * \file  app_config/proj_app_cfg.h
 * \brief application configuration
 */

#pragma once

#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

#if defined TEST_HOST
#include "app_config/host/proj_app_cfg.h"

#elif defined MCU_ESP32
#include "app_config/esp32/proj_app_cfg.h"

#else
#include "app_config/none/proj_app_cfg.h"
#error "no supported MCU"
#define MCU_ESP32
#endif

#ifdef __cplusplus
#include <utils_misc/cstring_utils.hh>
#define STRLCPY(dst,src,size) csu_copy((dst),(size),(src))
#define STRCPY(dst,src) csu_copy((dst),(src))
#else
#define STRLCPY(dst,src,size) strlcpy((dst),(src),(size))
#define STRCPY(dst,src) strcpy((dst),(src))
#endif

