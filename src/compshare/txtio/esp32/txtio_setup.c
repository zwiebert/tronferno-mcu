/*
 * txtio_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "txtio/inout.h"
#include <esp_system.h>
#include <esp32/rom/ets_sys.h>


static int es_io_putc(char c) {
  putchar(c);
  return 1;
}

static int  es_io_getc(void) {
 return getchar();
}

void txtio_setup() {

  io_putc_fun = es_io_putc;
  io_getc_fun = es_io_getc;
  io_printf_fun = ets_printf;
}
