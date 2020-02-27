/*
 * fer_app_cfg.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_FER_APP_CFG_H_
#define COMPONENTS_FERNOTRON_FER_APP_CFG_H_

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

#define FER_TRANSMITTER
#define FER_RECEIVER
#define ACCESS_GPIO
#define POSIX_TIME 1
#define MDR_TIME
#define USE_PAIRINGS
#define USE_JSON
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

#define USE_STATIC_PINS

#ifdef USE_STATIC_PINS
void mcu_put_txPin(uint8_t level);
uint8_t   mcu_get_rxPin();
#define FER_GET_RX_PIN  mcu_get_rxPin
#define FER_PUT_TX_PIN mcu_put_txPin
#endif

#endif /* COMPONENTS_FERNOTRON_FER_APP_CFG_H_ */
