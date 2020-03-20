#ifndef DEBUG_H_
#define DEBUG_H_

#include "app_config/proj_app_cfg.h"

extern int unit_testing;
#define UNIT_TEST (unit_testing+0)


#ifdef MCU_ESP8266
#define assert(x) do { if (x) {} else { ets_printf("assert failed: %s at %s:%d\n", __func__, __FILE__, __LINE__);mcu_restart(); }} while(0)
#else
#include <assert.h>
#endif

#ifdef MCU_ESP32
#include <esp32/rom/ets_sys.h>
#define db_puts(s) ets_printf("%s\n",s)
#define db_printf ets_printf
#endif

void abort_precond(const char *msg, int line);
void abort_postcond(const char *msg, int line);

#if defined NO_PRECOND || defined DISTRIBUTION
#define precond(x)
#else
#define precond(x)  assert(x)
#endif

#if defined NO_POSTCOND  || defined DISTRIBUTION
#define postcond(x)
#else
#define postcond(x) assert(x)
#endif

#define dbg_trace()

#ifndef dbg_printf
#define dbg_printf ets_printf
#endif
#ifndef dbg_trace
#define dbg_trace()   dbg_printf("trace: %s()\n", __func__)
#endif

#ifndef DISTRIBUTION
#define dbg_dbd(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)
#else
#define dbg_dbd(x)
#endif

#define if_verbose_do(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)

#if defined db_printf && ! defined DISTRIBUTION
#include "config/config.h"
#define dbg_vpf(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)
#else
#define dbg_vpf(x)
#endif



#endif /* DEBUG_H_ */
