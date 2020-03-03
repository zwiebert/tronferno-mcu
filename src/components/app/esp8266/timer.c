/*
 * timer.c
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

#include "config/config.h"
#include "main/rtc.h"
#include "fernotron/fer_timings.h"
#include "fernotron/int_timer.h"



void  mcu_delay_us(u16 us) {
  os_delay_us(us);
}

#define F_CPU 80000000UL
//////////
///////////////////////////////////////////////////////////
// definitions for RTC Timer1
#define TIMER1_DIVIDE_BY_1              0x0000
#define TIMER1_DIVIDE_BY_16             0x0004
#define TIMER1_DIVIDE_BY_256            0x0008

#define TIMER1_AUTO_LOAD                0x0040
#define TIMER1_ENABLE_TIMER             0x0080
#define TIMER1_FLAGS_MASK               0x00cc

#define TIMER1_NMI                      0x8000

#define TIMER1_COUNT_MASK               0x007fffff        // 23 bit timer

void 
timer1Start(u32 ticks, u16 flags, void (*handler)(void)) {
  RTC_REG_WRITE(FRC1_LOAD_ADDRESS, ticks & TIMER1_COUNT_MASK);
  RTC_REG_WRITE(FRC1_CTRL_ADDRESS, (flags & TIMER1_FLAGS_MASK) | TIMER1_ENABLE_TIMER);
  RTC_CLR_REG_MASK(FRC1_INT_ADDRESS, FRC1_INT_CLR_MASK);
  if (handler != NULL) {
    if (flags & TIMER1_NMI)
      ETS_FRC_TIMER1_NMI_INTR_ATTACH(handler);
    else
      ETS_FRC_TIMER1_INTR_ATTACH((void (*)(void *) )handler, NULL);
    TM1_EDGE_INT_ENABLE();
    ETS_FRC1_INTR_ENABLE();
  }
}

void 
timer1Stop(void) {
  ETS_FRC1_INTR_DISABLE();
  TM1_EDGE_INT_DISABLE();
  RTC_REG_WRITE(FRC1_CTRL_ADDRESS, 0);
}
//////////////////////////////////////////////////////////////////////////
volatile u32 run_time_s10;

void IRAM_ATTR timer_handler(void) {

#ifdef FER_TRANSMITTER
  if (transmTick == C.app_transm) {
    {
      static uint_fast8_t tick_count;
      if (0 == (++tick_count & (INTR_TICK_FREQ_MULT - 1))) {
        ftx_tick();
      }
    }
  }
#endif

#ifdef FER_RECEIVER
  if (recvTick == C.app_recv) {
    frx_tick();
  }
#endif

  {
    static u32 s10_ticks;

    if (s10_ticks-- == 0) {
      s10_ticks = TICK_FREQ_HZ / 10;
      ++run_time_s10;
    }
  }

  if (rtcAvrTime == C.app_rtc) {
    static u32 rtc_ticks;

    if (rtc_ticks-- == 0) {
      rtc_ticks = TICK_FREQ_HZ + MS_TO_TICKS(C.app_rtcAdjust);
      rtc_tick()
      ;
    }
  }
}

void  intTimer_setup(void) {
  u32 ticks = F_CPU / TICK_FREQ_HZ;
  u16 flags = TIMER1_DIVIDE_BY_1 | TIMER1_ENABLE_TIMER | TIMER1_AUTO_LOAD;

  timer1Start(ticks, flags, timer_handler);
}
