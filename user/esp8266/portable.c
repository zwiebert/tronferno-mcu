/*
 * portable.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

//typedef unsigned char bool;
#include <c_types.h>



#include <mem.h>

#include <stdio.h>
#include <string.h>

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"

#include "main/inout.h"

char * ICACHE_FLASH_ATTR
itoa(int val, char *s, int radix) {
	if (s)
		os_sprintf(s, (radix == 10 ? "%i" : "%x"), val);
	return s;
}

char * ICACHE_FLASH_ATTR
ltoa(long val, char *s, int radix) {
	if (s)
		os_sprintf(s, (radix == 10 ? "%li" : "%lx"), val);
	return s;

}
