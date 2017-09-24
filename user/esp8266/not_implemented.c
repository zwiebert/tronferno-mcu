/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"
#include <mem.h>

#include "../main/inout.h"
#include "../main/fer.h"
#include "../main/config.h"
#include "../main/time.h"
#include "config.h"

#include "esp_missing_includes.h"

#include <sntp.h>

extern int ets_uart_printf(const char *fmt, ...);
#define printf ets_uart_printf



void abort_precond(const char *msg, int orig_line) {
}
void abort_postcond(const char *msg, int orig_line) {
}
void ICACHE_FLASH_ATTR db_test_all_indicators(uint8_t nmb_flashs) {
}
void db_put_light(int state, int n) {
}

void ICACHE_FLASH_ATTR setup_notImplemented(void) {
	static int once;
#if 0
	if (once == 0) {
		once = 1;
		ip_addr_t *addr = (ip_addr_t *) os_zalloc(sizeof(ip_addr_t));
		sntp_setservername(0, "us.pool.ntp.org"); // set server 0 by domain name
		sntp_setservername(1, "ntp.sjtu.edu.cn"); // set server 1 by domain name
		ipaddr_aton("210.72.145.44", addr);
		sntp_setserver(2, addr); // set server 2 by IP address
		sntp_set_timezone((int)0);
		sntp_init();
		os_free(addr);

	}
	//                                             1505957550
	set_system_time(sntp_get_current_timestamp() -  946684800);
	printf("ntp stamp: %lu, %s\n", sntp_get_current_timestamp(), sntp_get_real_time(sntp_get_current_timestamp()));
#endif
}
