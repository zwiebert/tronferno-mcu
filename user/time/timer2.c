/*
    Set up the RTC based on using the AVR Timer 2 attached Clock Crystal.
    Developed for the Goldilocks Analogue, but also relevant for the Arduino Mega
*/
#include <stdlib.h>

#if 0
#include <util/atomic.h>
#include <avr/interrupt.h>

#include "time.h"

void
setup_RTC_interrupt( void )
{


	/* Using 8bit Timer2 to generate the tick.
	 * A 32.768 KHz crystal must be attached to the appropriate pins.
	 * We then adjust the scale factor and counter to roll over at the top
	 * so we can get EXACT seconds for the Real Time clock.
	 *
	 * This code is correct for an ATmega328p (Arduino Uno) but will fail to function
	 * because the pins are used for an external crystal.
	 */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
    	TIMSK2 &= ~( _BV(OCIE2B)|_BV(OCIE2A)|_BV(TOIE2) );	// disable all Timer2 interrupts
    	TIFR2 |=  _BV(OCF2B)|_BV(OCF2A)|_BV(TOV2);		// clear all pending interrupts
        ASSR = _BV(AS2);              				// set Timer/Counter2 to be asynchronous from the CPU clock
                               					// with a second external clock (32,768kHz) driving it.
        TCNT2  = 0x00;				 		// zero out the counter
        TCCR2A = 0x00;						// Normal mode
        TCCR2B = _BV(CS22) | _BV(CS20);				// divide timer clock by 128 so counter will roll over at MAX
    
        while( ASSR & (_BV(TCN2UB)|_BV(OCR2AUB)|_BV(TCR2AUB))); // Wait until Timer2 update complete
    
        /* Enable the interrupt - this is okay as interrupts are currently globally disabled. */
        TIMSK2 |= _BV(TOIE2);					// When the TOIE2 bit is written to one, the interrupt is enabled
    }
}

/*
 * Tick ISR for the RTC.  All this does is increment the RTC tick count, once per second.
 * Use ISR_NOBLOCK where there is an important timer running, that should preempt the RTC.
 * As long as it completes within one second, then there is no issue.
 */
ISR(TIMER2_OVF_vect, ISR_NAKED ISR_NOBLOCK ) __attribute__ ((hot, flatten));
ISR(TIMER2_OVF_vect)
{
	system_tick();
	reti();
}

#endif

