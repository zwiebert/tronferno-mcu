/*
 * cli_app_config.h
 *
 *  Created on: 19.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_CLI_CLI_APP_CONFIG_H_
#define COMPONENTS_CLI_CLI_APP_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;

#define USE_MUTEX


#if defined MCU_ESP8266
#include "../../esp8266/esp8266_user_config.h"
#elif defined MCU_ESP32
#include "app/esp32/esp32_user_config.h"
#else
#error "no supported MCU"
#define IRAM_ATTR
#endif

#ifdef DISTRIBUTION
#define assert(x)
#define precond(x)
#define postcond(x)
#elif defined MCU_ESP8266
#define assert(x)
#define precond(x)
#define postcond(x)
#elif defined MCU_ESP32
#include <assert.h>
#define precond(x) assert(x)
#define postcond(x) assert(x)
#else
#define assert(x)
#define precond(x)
#define postcond(x)
#endif


#endif /* COMPONENTS_CLI_CLI_APP_CONFIG_H_ */
