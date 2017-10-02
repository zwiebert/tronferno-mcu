#ifndef _common_h
#define _common_h

#ifndef AVR  // FIXME: how to recognize ESP8266 
#define MCU_ESP8266
#endif

////////////////////////
// disable features not working on some MCUs
#ifdef MCU_ESP8266
//#define FER_RECEIVER
//#define FER_RECEIVER_MINIMAL  // not enough IRAM on ESP8266 (disables timer receiving capability)
#endif

////////////////////////////
// MCU dependent compiler attributes in common code
#ifdef MCU_ESP8266
#include "user_interface.h"
#else
#define ICACHE_FLASH_ATTR
#endif

#ifdef AVR
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif
////////////////////////

#include <stdint.h>
#include <stdbool.h>

typedef bool logicLevel;
#define LOW false
#define HIGH true

#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define IS_IN_RANGE(lowLim, val, highLim) (((lowLim) < (val)) && ((val) < (highLim)))

#define GET_BYTE_0(src) (((src)>>0)&0xff)
#define GET_BYTE_1(src) (((src)>>8)&0xff)
#define GET_BYTE_2(src) (((src)>>16)&0xff)
#define GET_BYTE_3(src) (((src)>>24)&0xff)

#define PUT_LOW_NIBBLE(dst, val) ((dst) = ((dst)&0xf0)|((val)&0x0f))
#define PUT_HIGH_NIBBLE(dst, val) ((dst) = (((val)<<4)&0xf0)|((dst)&0x0f))
#define GET_LOW_NIBBLE(src) ((src)&0x0f)
#define GET_HIGH_NIBBLE(src) (((src)>>4)&0x0f)

#define PUT_LOW_BYTE(dst, val) ((dst) = ((dst)&0xff00)|((val)&0x00ff))
#define PUT_HIGH_BYTE(dst, val) ((dst) = (((val)<<8)&0xff00)|((dst)&0x00ff))
#define GET_LOW_BYTE(src) ((src)&0x00ff)
#define GET_HIGH_BYTE(src) (((src)>>8)&0x00ff)

#define GET_BIT(var,pos) ((var) & (1<<(pos)))



#if 0
#define NO_PRECOND
#define NO_POSTCOND
#endif

#include "debug.h"

#endif // _common_h
