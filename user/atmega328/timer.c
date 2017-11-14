#include <avr/interrupt.h>

#include "main/common.h"
#include "main/rtc.h"
#include "main/fer.h"
#include "main/rtc.h"
#include "main/config.h"
#include "main/rtc.h"
#include <util/delay.h>


void mcu_delay_us(uint16_t us) {
	_delay_us(100);  // FIXME: needs constant
}


ISR(TIMER1_COMPA_vect) {

#ifdef FER_TRANSMITTER
	if (transmTick == C.app_transm) {
#ifdef FER_SENDER_DCK
		{
			static uint_fast8_t tick_count;
			if (0 == (++tick_count & (INTR_TICK_FREQ_MULT - 1))) {
				tick_ferSender();
			}

		}
#else
		tick_ferSender();
#endif
	}
#endif

#ifdef FER_RECEIVER
	if (recvTick == C.app_recv) {
		tick_ferReceiver();
	}
#endif
#if 1
	if (rtcAvrTime == C.app_rtc) {
		static uint32_t rtc_ticks;

		if (rtc_ticks-- == 0) {
			rtc_ticks = TICK_FREQ_HZ + MS_TO_TICKS(C.app_rtcAdjust);
			rtc_tick();
		}
	}
#endif

	postcond(TCNT1 < OCR1A);
}

void setup_timer() {
	// CS bits to select clock source: none/mainOsc/mainOscPrescaled/extClock
	//(none, osc) == (CS:000, 001)
	//(prescaler:8,64,256,1024) == (CS:010,011,100,101)
	//(extClk_fall, extClk_rise) == (CS:110,111)
	TCCR1A = 0x00;
	TCCR1B = (1 << WGM12) | ((0 << CS12) | (0 << CS11) | (1 << CS10));

	// set counter value to reach for interrupt and clearing counter
	OCR1A = F_CPU / TICK_FREQ_HZ;
	// enable output compare interrupt A
	TIMSK1 = _BV(OCIE1A);
	// current value: 0,1 ms, 10 kHz
}

