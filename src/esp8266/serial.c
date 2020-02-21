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

#include "txtio/inout.h"
#include "driver/uart.h"




extern int ets_uart_printf(const char *fmt, ...);

static int es_io_putc(char c) {
  ets_uart_printf("%c", c);
  return 1;
}

#define RX_BUFSIZE 128
static u8 buf[RX_BUFSIZE];
static u8 head = 0, tail = 0;

void rx_copy(u8 *start, u8 *end) {
  while (start < end) {
    buf[tail++] = *start++;
    tail %= RX_BUFSIZE;
  }
}

static int  es_io_getc(void) {
  int result = -1;

  if (head != tail) {
    result = buf[head++] & 0xFF;
    head %= RX_BUFSIZE;
  }

  return result;
}

void 
setup_serial(u32 baudrate) {
  // Configure the UART
  uart_init(baudrate, baudrate); // FIXME: the baud_rate parameter has no effect
  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  io_printf_fun = ets_uart_printf;
}
