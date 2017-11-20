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
void setup_dataFlash2(void);
void setup_ntp(void);
void setup_spiffs(void);

// main loop task
#define user_procTaskPrio     2
#define user_procTaskQueueLen 1
static os_event_t user_procTaskQueue[user_procTaskQueueLen];

// io task
#define io_procTaskPrio     0
#define io_procTaskQueueLen 1
static os_event_t io_procTaskQueue[io_procTaskQueueLen];

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events) {
	wifiStation_loop();
	loop();
	ets_delay_us(1000);
	system_os_post(user_procTaskPrio, 0, 0);
}

static void ICACHE_FLASH_ATTR
io_procTask(os_event_t *events) {
	wifiStation_loop();
	ets_delay_us(1000);
	system_os_post(io_procTaskPrio, 0, 0);
}

// hardware specific main setup
void ICACHE_FLASH_ATTR
user_init() {

	setup_dataFlash();

	read_config();

	setup_serial(C.mcu_serialBaud);

	//  setup_serial(C.serialBaud);
	setup_notImplemented();
#ifdef USE_NTP
	setup_ntp();
#endif
	setup_pin();
	setup_wifistation();

	main_setup();

	system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
	system_os_post(user_procTaskPrio, 0, 0);
#if 0
	system_os_task(io_procTask, io_procTaskPrio, io_procTaskQueue, io_procTaskQueueLen);
	system_os_post(io_procTaskPrio, 0, 0 );
#endif

	setup_timer();

	setup_dataFlash2();
	setup_spiffs();

	struct rst_info *rtc_info = system_get_rst_info();
	if (rtc_info) {
		printf("reset reason: %x\n", rtc_info->reason);
		if (rtc_info->reason == REASON_WDT_RST || rtc_info->reason == REASON_EXCEPTION_RST || rtc_info->reason == REASON_SOFT_WDT_RST) {
			if (rtc_info->reason == REASON_EXCEPTION_RST) {
				printf("Fatal exception (%d):\n", rtc_info->exccause);
			}
			printf("epc1=0x%08x, epc2=0x%08x, epc3=0x%08x, excvaddr=0x%08x,	depc=0x%08x\n", rtc_info->epc1, rtc_info->epc2, rtc_info->epc3, rtc_info->excvaddr,
					rtc_info->depc); //The address of the last crash is printed, which is used to debug garbled	output.
		}
	}
#ifdef DEBUG
	gdbstub_init();
#endif
	// system_set_os_print(1);
	//system_print_meminfo();

}

void ICACHE_FLASH_ATTR mcu_restart(void) {
	printf("mcu_restart()\n");
	os_delay_us(100000);
	system_restart();
#if 0
	os_delay_us(10000);  // FIXME: is calling this function allowed?
#endif
	for (;;) {
	};
}
