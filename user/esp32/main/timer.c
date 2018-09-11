/*
 * timer.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"

#include "esp_sleep.h"
#include "sdkconfig.h"

#include "main/inout.h"
#include "main/fer.h"
#include "main/config.h"
#include "main/rtc.h"



void ICACHE_FLASH_ATTR mcu_delay_us(uint16_t us) {
  ets_delay_us(us);
}

static void timer1_handler(void *args);
esp_timer_handle_t timer1_handle;

void timer1_start() {
      const esp_timer_create_args_t periodic_timer_args = {
              .callback = &timer1_handler,
              /* name is optional, but may help identify the timer when debugging */
              .name = "timer1"
      };

      ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &timer1_handle));
      ESP_ERROR_CHECK(esp_timer_start_periodic(timer1_handle, TICK_PERIOD_US));
}

void ICACHE_FLASH_ATTR
timer1Stop(void) {
  ESP_ERROR_CHECK(esp_timer_stop(timer1_handle));

}
//////////////////////////////////////////////////////////////////////////

static void timer1_handler(void *args) {

#ifdef FER_TRANSMITTER
  if (transmTick == C.app_transm) {
#ifdef FER_SENDER_DCK
    {
      static uint_fast8_t tick_count;
      if (0 == (++tick_count & (INTR_TICK_FREQ_MULT - 1))) {
        ftx_tick();
      }

    }
#else
    ftx_tick();
#endif
  }
#endif

#ifdef FER_RECEIVER
  if (recvTick == C.app_recv) {
    frx_tick();
  }
#endif

  if (rtcAvrTime == C.app_rtc) {
    static uint32_t rtc_ticks;

    if (rtc_ticks-- == 0) {
      rtc_ticks = TICK_FREQ_HZ + MS_TO_TICKS(C.app_rtcAdjust);
      rtc_tick()
      ;
    }
  }

}

void ICACHE_FLASH_ATTR setup_timer(void) {
  timer1_start();
}

