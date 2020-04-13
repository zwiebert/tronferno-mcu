#ifndef _common_h
#define _common_h

////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include "misc/int_types.h"

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
