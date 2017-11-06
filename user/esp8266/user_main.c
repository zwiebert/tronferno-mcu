/*
 * user_main.c
 *
 *  Created on: 11.09.2017
 *      Author: bertw
 */

#include "esp_missing_includes.h"

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"

#include "main/common.h"
#include "main/inout.h"
#include "main/config.h"
#ifdef DEBUG
#include <../../gdbstub/gdbstub.h>
#endif

#define printf ets_uart_printf

extern void loop(void);
extern void wifiStation_loop(void);


extern void setup_timer(void);
extern void setup_serial(uint32_t baudrate);
extern void setup_notImplemented(void);
extern void setup_pin(void);
void main_setup(void);
void setup_wifistation(void);
void setup_dataFlash(void);
void setup_ntp(void);




// main loop task
#define user_procTaskPrio     2
#define user_procTaskQueueLen 1
static os_event_t user_procTaskQueue[user_procTaskQueueLen];

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{
	wifiStation_loop();
    loop();
    ets_delay_us(1000);
    system_os_post(user_procTaskPrio, 0, 0 );
}


// hardware specific main setup
void ICACHE_FLASH_ATTR
user_init() {

    setup_dataFlash();
	read_config();

    setup_serial(C.mcu_serialBaud);


  //  setup_serial(C.serialBaud);
    setup_notImplemented();
#ifdef FER_NTP
    setup_ntp();
#endif
    setup_pin();
    setup_wifistation();


	main_setup();

    system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
    system_os_post(user_procTaskPrio, 0, 0 );

    setup_timer();


#ifdef DEBUG
	gdbstub_init();
#endif
   // system_set_os_print(1);
    //system_print_meminfo();

}

void ICACHE_FLASH_ATTR mcu_restart(void) {
	system_restart();
#if 0
	os_delay_us(10000);  // FIXME: is calling this function allowed?
#endif
	for(;;) {};
}
