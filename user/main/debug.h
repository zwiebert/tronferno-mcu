#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef MCU_ESP32
#include <rom/ets_sys.h>
#define db_puts(s) ets_printf("%s\n",s)
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


#ifdef NO_PRECOND
#define precond(x)
#else
#define precond(x)  ((x) ? (void)0 : abort_precond("", __LINE__))
#endif


#ifdef NO_POSTCOND
#define postcond(x)
#else
#define postcond(x) ((x) ? (void)0 : abort_postcond("", __LINE__))
#endif




#define dbg_trace()
#define dbg_printf #error

#ifndef dbg_printf
#define dbg_printf io_printf
#endif
#ifndef dbg_trace
#define dbg_trace()   dbg_printf("trace: %s()\n", __func__)
#endif


// testing modules
#define TEST_MODULE_ASTRO 0
#define TEST_MODULE_RTC 0
#define TEST_MODULE_FER_PRG 0
#define TEST_MODULE_CONFIG 0
#define TEST_MODULE_CLI 0

bool test_modules(void);
bool testModule_astro(void);
bool testModule_rtc(void);
bool testModule_fer_prg(void);
bool testModule_config(void);
bool testModule_cli(void);


#endif /* DEBUG_H_ */
