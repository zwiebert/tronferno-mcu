#include <stdlib.h>

#include "main/debug.h"
#include "main/inout.h"
#include "main/common.h"

#ifndef DB_NO_LIGHTS


#include  <util/delay.h>
#include <avr/interrupt.h> 
#include <avr/io.h>


#include "defines.h"


#define LED_PRE (D,7)
#define LED_POST (D,6)
#define LED_BLINK LED_PRE
#define LED_1 LED_PRE
#define LED_2 LED_POST
#define LED_3 LED_POST

void db_put_light(int state, int n) {
  if (n==1)
    PUTPIN(LED_1, state != 0);
  if (n==2)
    PUTPIN(LED_2, state != 0);
  if (n==3)
    PUTPIN(LED_3, state != 0);
}


void db_toggle_light(int state, int n) {
  if (n==1)
    TGLPIN(LED_1);
  if (n==2)
    TGLPIN(LED_2);
  if (n==3)
    TGLPIN(LED_3);
}


void
db_toggle_def() {
  TGLPIN(LED_BLINK);
}

void
db_put_def(int state) {
  PUTPIN(LED_BLINK, state != 0);
}

void db_test_all_indicators(uint8_t nmb_flashs) {
  uint8_t i;
  SET_DDR_OUT(LED_PRE);
  SET_DDR_OUT(LED_BLINK);
  SET_DDR_OUT(LED_POST);

  for (i=0;;++i) {
    PUTPIN(LED_PRE, i&1);
    PUTPIN(LED_POST, i&1);
    PUTPIN(LED_BLINK, i&1);
    if (i >= (nmb_flashs * 2))
      break;
    _delay_ms(250);
  }

}



void 
db_blink_def(int ms_on, int ms_off, int repeats) {
int i;
  SET_DDR_OUT(LED_BLINK);

  for (;;) {
    SETPIN(LED_BLINK);
    for(i=ms_on; i > 0; --i) _delay_ms(1);
    CLRPIN(LED_BLINK);
    if (--repeats < 0)
      break;
    for(i=ms_off; i > 0; --i) _delay_ms(1);
  }
}



#ifndef NO_PRECOND

void abort_precond(const char *msg, int orig_line)
{
  int repeats = 5;

  while (repeats-- > 0) { 
  
    int i, n=10000, line = orig_line;


    //io_puts(msg); io_puts("\n");

    cli();
    SET_DDR_OUT(LED_PRE);


    while (n > 1) {
      line %= n;
      n /= 10;

      if (orig_line > n) {
	_delay_ms(2000);
	SETPIN(LED_PRE);
	_delay_ms(1000);
	CLRPIN(LED_PRE);
	_delay_ms(2000);
      }

      for(i=(line/n); i > 0; --i) {
	SETPIN(LED_PRE);
	_delay_ms(200);
	CLRPIN(LED_PRE);
	_delay_ms(300);
      }
    }

    _delay_ms(2000);
    SETPIN(LED_PRE);
    _delay_ms(10000);
  }

  abort();
}
#endif

#ifndef NO_POSTCOND
void abort_postcond(const char *msg, int orig_line)
{
  int repeats = 5;

  while (repeats-- > 0) { 
  
    int i, n=10000, line = orig_line;
    //io_puts(msg); io_puts("\n");

    cli();
    SET_DDR_OUT(LED_POST);

    while (n > 1) {
      line %= n;
      n /= 10;

      if (orig_line > n) {
	_delay_ms(2000);
	SETPIN(LED_POST);
	_delay_ms(1000);
	CLRPIN(LED_POST);
	_delay_ms(2000);
      }

      for(i=(line/n); i > 0; --i) {
	SETPIN(LED_POST);
	_delay_ms(200);
	CLRPIN(LED_POST);
	_delay_ms(300);
      }
    }

    _delay_ms(2000);
    SETPIN(LED_POST);
    _delay_ms(10000);
  }


  abort();
}

#endif
#endif // NO_LIGHTS



