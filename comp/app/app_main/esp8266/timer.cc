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

#include "app_settings/config.h"
#include "app_misc/rtc.h"
#include "fernotron_trx/raw/fer_radio_timings.h"
#include "fernotron_trx/isr_timer_config.h"
#include "fernotron_trx/raw/fer_radio_trx.h"



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
volatile u32 run_time_s_, run_time_ts_;

void IRAM_ATTR timer_handler(void) {
#ifdef FER_TRANSMITTER
  fer_tx_setOutput();
#endif
#ifdef FER_RECEIVER
  void fer_rx_sampleInput();
#endif

#ifdef FER_TRANSMITTER
  {
    static uint_fast8_t tick_count;
    if (0 == (++tick_count & (INTR_TICK_FREQ_MULT - 1))) {
      fer_tx_dck();
    }
  }
#endif

#ifdef FER_RECEIVER
  fer_rx_tick();
#endif

  {
    static u32 s10_ticks = TICK_FREQ_HZ / 10;
    static int8_t s_ticks = 10;

    if (s10_ticks-- == 0) {
      s10_ticks = TICK_FREQ_HZ / 10;
      ++run_time_ts_;
      if (!s_ticks--) {
        s_ticks = 10;
        ++run_time_s_;

#ifndef USE_POSIX_TIME
        extern volatile time_t __system_time;
++__system_time;
#endif

      }
    }
  }
}


void  app_timerISR_setup(void) {
  u32 ticks = F_CPU / TICK_FREQ_HZ;
  u16 flags = TIMER1_DIVIDE_BY_1 | TIMER1_ENABLE_TIMER | TIMER1_AUTO_LOAD;

  timer1Start(ticks, flags, timer_handler);
}

