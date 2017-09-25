#include <avr/interrupt.h>

#include "../main/common.h"
#include "timer.h"
#include "../main/fer.h"
//#include "codec.h"
//#include "inout.h"
#include "../main/rtc.h"
#include "../main/config.h"
#include "time.h"


ISR(TIMER1_COMPA_vect)
{
 
 #ifdef FER_TRANSMITTER
 // if (transmTick == C.app_transm)
  {
    tick_ferSender();
  }
  #endif
 
 #ifdef FER_RECEIVER 
  if (recvTick == C.app_recv)
  {
    tick_ferReceiver();
  }
 #endif
 #if 1
  if (rtcAvrTime == C.app_rtc)
  {
    static uint32_t rtc_ticks;
    if (rtc_ticks-- == 0) {
       rtc_ticks = TICK_FREQ_HZ + (C.app_rtcAdjust * 2 * INTR_TICK_FREQ_MULT);
       system_tick();
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
  TCCR1B = (1<<WGM12) | ((0<<CS12)|(0<<CS11)|(1<<CS10));

  // set counter value to reach for interrupt and clearing counter
  OCR1A  = F_CPU / TICK_FREQ_HZ;
  // enable output compare interrupt A
  TIMSK1 = _BV(OCIE1A);
  // current value: 0,1 ms, 10 kHz
}

