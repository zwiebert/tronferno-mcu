/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include <user_interface.h>
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"
#include <mem.h>

#include "fernotron/fer.h"
#include "config/config.h"
#include <time.h>

#include "../userio/inout.h"
#include "config/config.h"





#define printf ets_uart_printf



void abort_precond(const char *msg, int orig_line) {
}
void abort_postcond(const char *msg, int orig_line) {
}
void ICACHE_FLASH_ATTR db_test_all_indicators(u8 nmb_flashs) {
}
void db_put_light(int state, int n) {
}

void ICACHE_FLASH_ATTR setup_notImplemented(void) {

}
