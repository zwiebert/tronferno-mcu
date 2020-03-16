#ifndef DEBUG_H_
#define DEBUG_H_

#include "app/proj_app_cfg.h"

extern int unit_testing;
#define UNIT_TEST (unit_testing+0)


#ifdef MCU_ESP8266
// FIXME: don't want link an entire libc just for assert()
#define assert(x) do { if (x) {} else { mcu_restart(); }} while(0)
//#define assert(x)
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

#if 0
#define db_puts(s) io_puts(s)
#endif

#ifdef DISTRIBUTION
#undef db_puts
#define db_puts(s)
#endif

#ifndef DB_NO_LIGHTS
void db_blink_def(int ms_on, int ms_off, int repeats);
void db_toggle_def(void);
void db_put_def(int state);
void db_put_light(int state, int n);
void db_toggle_light(int state, int n);
void db_blink_light(int ms_on, int ms_off, int repeats, int n);


/* to turn on indicator lights at program start to see if they are
   connected properly */
void db_test_all_indicators(uint8_t nmb_flashs);
#else

#define db_blink_def(ms_on, ms_off, repeats)
#define db_toggle_def()
#define db_put_def( state)
#define db_put_light( state,  n)
#define db_toggle_light( state,  n)
#define db_blink_light( ms_on,  ms_off,  repeats,  n)
#define db_test_all_indicators(nmb_flashs)
#endif


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
#define dbg_printf #error

#ifndef dbg_printf
#define dbg_printf io_printf
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
