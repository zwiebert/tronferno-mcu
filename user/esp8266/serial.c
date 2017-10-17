/*
 * serial.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"

#include "main/inout.h"

#include "esp_missing_includes.h"

extern int ets_uart_printf(const char *fmt, ...);

static int es_io_putc(char c) {
	ets_uart_printf("%c", c);
	return 1;
}

#define RX_BUFSIZE 256
static uint8_t buf[RX_BUFSIZE];
static uint8_t head = 0, tail = 0;

void rx_copy(uint8_t *start, uint8_t *end) {
	while (start < end) {
		buf[tail++] = *start++;
		tail %= RX_BUFSIZE;
	}
}

static int ICACHE_FLASH_ATTR es_io_getc(void) {
	int result = -1;

	if (head != tail) {
		result = buf[head++] & 0xFF;
		head %= RX_BUFSIZE;
	}

	return result;
}

void ICACHE_FLASH_ATTR
setup_serial(uint32_t baudrate) {
	// Configure the UART
	uart_init(baudrate, baudrate); // FIXME: the baud_rate parameter has no effect
	io_putc_fun = es_io_putc;
	io_getc_fun = es_io_getc;
	io_printf_fun = ets_uart_printf;
}
