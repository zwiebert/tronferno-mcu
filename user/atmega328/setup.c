/*
 * setup.c
 *
 * Created: 12.09.2017 01:46:57
 *  Author: bertw
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "../main/config.h"
#include "../main/inout.h"
#include "rs232.h"

void setup_pin(void);
void setup_timer(void);

void setup_mcu(void)
{
   read_config();
   
   setup_pin();

   init_usart(C.mcu_serialBaud);
   init_usart_rx_buffer();
   io_putc_fun = ser_putc_block;
   io_getc_fun = ser_getc_nonblock;
  
  #ifndef DEBUGGER
  // interrupts
  setup_timer();
  #endif
  
  sei();
}
