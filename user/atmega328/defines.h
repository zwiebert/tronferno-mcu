#ifndef defines_h__
#define defines_h__

/* -*- C -*- */


#include <avr/io.h>

/********** Program specific defines ***********************/

/***********************************************************/

/* Alternate port access macros from avr-gcc list */
/* http://lists.gnu.org/archive/html/avr-gcc-list/2004-12/msg00097.html */
/* Dave Hylands */

#if 0
//Example defines
#define DATA1         (D, 7)
#define DATA2         (C, 3)
#define DATA3         (B, 3)
#endif

/* Code to manipulate PORTx, PINx, and DDRx using definitions like those above */
#define PORT(port,pin)     (PORT##port)
#define DDR(port,pin)      (DDR##port)
#define PIN(port,pin)      (PIN##port)
#define PIN_MASK(port,pin) (1<<(pin))

/* Set a pin */
#define SETPIN(pindef)      PORT pindef |= PIN_MASK pindef

/* Clear a pin */
#define CLRPIN(pindef)      PORT pindef &= ~PIN_MASK pindef

/* Toggle a pin */
#define TGLPIN(pindef)      PORT pindef ^= PIN_MASK pindef

/* Set or clear pin according to parameter VAL */
#define PUTPIN(pindef,val)  ((val) ? (PORT pindef |= PIN_MASK pindef) : (PORT pindef &= ~PIN_MASK pindef))

/* Set a IO to output */
#define SET_DDR_OUT(pindef) DDR pindef |= PIN_MASK pindef

/* Set a IO to input */
#define SET_DDR_IN(pindef)  DDR pindef &= ~PIN_MASK pindef

/* Read the value of a pin - a byte with the correspondig bit set or cleard is returned.
   Example:

   #define PIN7 (D, 7)
   ...
   SETPIN(PIN7);
   x = GETPIN(PIN7);
  { x = 128 }
*/
#define GETPIN(pindef)      (PIN pindef & PIN_MASK pindef)

/*
Example usage:

  SET_DDR_OUT(DATA1);
  SET_DDR_IN(DATA2);
  SETPIN(DATA1);
  CLRPIN(DATA1);
  x = GETPIN(DATA2);
  PUTPIN(DATA1, (x != 0));

*/

#endif // defines_h__
