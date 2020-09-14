/*
 * serial.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>

#include "txtio/inout.h"
#include "driver/uart.h"

extern int ets_uart_printf(const char *fmt, ...);


int es_io_putc(char c) {
  uart_tx_one_char(c);
  return 1;
  ets_uart_printf("%c", c);
  return 1;
}


int  es_io_getc(void) {
  return uart_rx_buf_getc();
  int result = -1;


  return result;
}

void 
setup_serial(u32 baudrate) {
  // Configure the UART
  uart_init((UartBaudRate)baudrate, (UartBaudRate)baudrate); // FIXME: the baud_rate parameter has no effect
}
