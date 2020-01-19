#ifndef _common_h
#define _common_h

////////////////////////
// MCU dependent compiler attributes in common code
#ifdef MCU_ESP8266
#include "user_interface.h"
#else
#define ICACHE_FLASH_ATTR
#endif

#ifdef MCU_ESP32
#include "esp_attr.h"
#else
#define IRAM_ATTR
#endif

////////////////////////

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;

typedef bool logicLevel;
#define LOW false
#define HIGH true

#ifdef DISTRIBUTION
#define NO_PRECOND
#define NO_POSTCOND
#endif

#include "debug/debug.h"

void mcu_delay_us(u16 us);
void mcu_restart(void);


// fernotron.c
void loop(void);
int main_setup(void);

#endif // _common_h
