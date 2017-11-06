/*
 * main_loop.c
 *
 * Created: 12.09.2017 02:19:53
 *  Author: bertw
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "main/inout.h"

void loop(void);
void main_setup(void);
void setup_mcu(void);

int 
main() {

setup_mcu();
io_puts("setup mcu done\n");
main_setup();

  for(;;) {
    sleep_mode();
    loop();
  }
}

void mcu_restart(void) {
    wdt_enable(WDTO_15MS);
    for(;;) {

    }
}

#if 1
// disable watch dog at start to avoid restart loop

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

// Function Implementation
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();

    return;
}

#endif
