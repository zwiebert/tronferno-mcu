/* -*- compile-command: "/cygdrive/e/WinAVR-20100110/utils/bin/make.exe -C default  all && bash -c ./__flash.sh"; -*- */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <avr/sleep.h>

#include "../main/common.h"
#include "rs232.h"

#define dprintf(format, args...)                        \
(snprintf(buf, sizeof(buf2), format , ## args), io_puts(buf))

#define NO_BLOCK 0


////////////////////////////////
//              Writing Buffer Stuff
#define tx_buffer_size 16
#define tx_buffer_mask (tx_buffer_size - 1)

volatile uint8_t tx_buffer_tail;
volatile uint8_t tx_buffer_head;
volatile uint8_t tx_buffer[tx_buffer_size];
volatile uint8_t tx_buffer_overflow_flag;

bool space_in_tx_buffer(void);
void write_to_tx_buffer(uint8_t);

////////////////////////////////
//              Reading Buffer Stuff

#define rx_buffer_size 8
#define rx_buffer_mask (rx_buffer_size - 1)

volatile uint8_t rx_buffer_tail;
volatile uint8_t rx_buffer_head;
volatile uint8_t rx_buffer[rx_buffer_size];
volatile uint8_t rx_buffer_overflow_flag;

bool data_in_rx_buffer(void);
uint8_t read_from_rx_buffer(void);
void init_usart_rx_buffer(void);
////////////////////////////////

void init_usart(unsigned long);


int
ser_putc_block(char c) {
  while(!space_in_tx_buffer()) {
    sleep_mode();
  }
  write_to_tx_buffer(c);
  return 0;
}

int
ser_putc_nonblock(char c) {
  while(!space_in_tx_buffer()) {
    return -1;
  }
  write_to_tx_buffer(c);
  return 0;
}

int
ser_getc_block() {
  while(!data_in_rx_buffer()) {
    sleep_mode();
  }
  return read_from_rx_buffer();
}

int
ser_getc_nonblock() {
  while(!data_in_rx_buffer()) {
    return -1;
  }
  return read_from_rx_buffer();
}







void init_usart (unsigned long baud)
{

  unsigned int UBRR_2x_off;
  unsigned int UBRR_2x_on;
  unsigned long closest_match_2x_off;
  unsigned long closest_match_2x_on;
  uint8_t off_2x_error;
  uint8_t on_2x_error;

  UBRR_2x_off = F_CPU/(16*baud) - 1;
  UBRR_2x_on = F_CPU/(8*baud) - 1;

  closest_match_2x_off = F_CPU/(16*(UBRR_2x_off + 1));
  closest_match_2x_on = F_CPU/(8*(UBRR_2x_on + 1));

  off_2x_error = 255*(closest_match_2x_off/baud - 1);
  if (off_2x_error <0) {off_2x_error *= (-1);}
  on_2x_error = 255*(closest_match_2x_on/baud -1);
  if (on_2x_error <0) {on_2x_error *= (-1);}

  if(baud > F_CPU / 16)
  {
    UBRR0L = 0xff & UBRR_2x_on;
    UBRR0H = 0xff & (UBRR_2x_on>>8);
    UCSR0A |= (1<<U2X0);
    } else {

    if (off_2x_error > on_2x_error)
    {
      UBRR0L = 0xff & UBRR_2x_on;
      UBRR0H = 0xff & (UBRR_2x_on>>8);
      UCSR0A |= (1<<U2X0);
      } else {
      UBRR0L = 0xff & UBRR_2x_off;
      UBRR0H = 0xff & (UBRR_2x_off>>8);
      UCSR0A &= ~(1<<U2X0);
    }
  }

  UCSR0B = (0<<RXCIE0) |
  (0<<TXCIE0) |
  (0<<UDRIE0) |
  (1<<RXEN0) |
  (1<<TXEN0) |
  (0<<UCSZ02);

  UCSR0A = (0<<U2X0) |
  (0<<MPCM0) ;

  UCSR0C = (0<<UMSEL01) | (0<<UMSEL00) |
  (0<<UPM01) | (0<<UPM00) |
  (0<<USBS0) |
  (1<<UCSZ01) | (1<<UCSZ00) |
  (0<<UCPOL0) ;
  
}

//////////////////////////////
//              Writing Buffer Stuff
#if 0
#define TX_BUF_IS_EMPTY()      (tx_buffer_head == tx_buffer_tail)
#define TX_BUF_IS_FULL()       (((tx_buffer_head+1)&tx_buffer_mask) == tx_buffer_tail)
#define TX_BUF_ADVANCE_HEAD()  (tx_buffer_head = ((tx_buffer_head + 1) & tx_buffer_mask))
#define TX_BUF_PUT(c)          (tx_buffer[tx_buffer_head] = c)
#define TX_BUF_ADVANCE_TAIL()  (tx_buffer_tail = ((tx_buffer_tail + 1) & tx_buffer_mask))
#define TX_BUF_GET()           (tx_buffer[tx_buffer_tail])
#else
#define TX_BUF_IS_EMPTY()      (((tx_buffer_head ^ tx_buffer_tail) & tx_buffer_mask) == 0)
#define TX_BUF_IS_FULL()       ((((tx_buffer_head+1) ^ tx_buffer_tail) & tx_buffer_mask) == 0)
#define TX_BUF_ADVANCE_HEAD()  (++tx_buffer_head)
#define TX_BUF_PUT(c)          (tx_buffer[(tx_buffer_head&tx_buffer_mask)] = c)
#define TX_BUF_ADVANCE_TAIL()  (++tx_buffer_tail)
#define TX_BUF_GET()           (tx_buffer[(tx_buffer_tail&tx_buffer_mask)])
#endif

bool space_in_tx_buffer(void)  { return !TX_BUF_IS_FULL(); }

void write_to_tx_buffer(uint8_t c)
{
  //cli();
  if(!TX_BUF_IS_FULL()) {
    TX_BUF_ADVANCE_HEAD();
    TX_BUF_PUT(c);
    UCSR0B |= (1<<UDRIE0);
  }
  //sei();
}

ISR(USART_UDRE_vect)
{
  if(TX_BUF_IS_EMPTY()) {
    UCSR0B &= ~(1<<UDRIE0);
    } else {
    TX_BUF_ADVANCE_TAIL();
    UDR0 = TX_BUF_GET();
  }
}

//////////////////////////////
//              Reading Buffer Stuff
#if 0
#define RX_BUF_IS_EMPTY()      (rx_buffer_head == rx_buffer_tail)
#define RX_BUF_IS_FULL()       (((rx_buffer_head+1)&rx_buffer_mask) == rx_buffer_tail)
#define RX_BUF_ADVANCE_HEAD()  (rx_buffer_head = ((rx_buffer_head + 1) & rx_buffer_mask))
#define RX_BUF_PUT(c)          (rx_buffer[rx_buffer_head] = c)
#define RX_BUF_ADVANCE_TAIL()  (rx_buffer_tail = ((rx_buffer_tail + 1) & rx_buffer_mask))
#define RX_BUF_GET()           (rx_buffer[rx_buffer_tail])
#else
#define RX_BUF_IS_EMPTY()      (((rx_buffer_head ^ rx_buffer_tail) & rx_buffer_mask) == 0)
#define RX_BUF_IS_FULL()       ((((rx_buffer_head+1) ^ rx_buffer_tail) & rx_buffer_mask) == 0)
#define RX_BUF_ADVANCE_HEAD()  (++rx_buffer_head)
#define RX_BUF_PUT(c)          (rx_buffer[(rx_buffer_head&rx_buffer_mask)] = c)
#define RX_BUF_ADVANCE_TAIL()  (++rx_buffer_tail)
#define RX_BUF_GET()           (rx_buffer[(rx_buffer_tail&rx_buffer_mask)])
#endif



void init_usart_rx_buffer(void)
{
  UCSR0B |= (1<<RXCIE0);
  //sei();
}

bool data_in_rx_buffer(void) { return !RX_BUF_IS_EMPTY(); }

uint8_t read_from_rx_buffer(void)
{
  uint8_t result = 0;
  //cli();
  if (!RX_BUF_IS_EMPTY()){
    RX_BUF_ADVANCE_TAIL();
    result = RX_BUF_GET();
  }
  //sei();
  return result;
}

ISR(USART_RX_vect)
{
  if(RX_BUF_IS_FULL())
  {
    rx_buffer_overflow_flag = UDR0;  // throw away!?
    rx_buffer_overflow_flag = 1;
    }   else    {
    RX_BUF_ADVANCE_HEAD();
    RX_BUF_PUT(UDR0);
  }
}


