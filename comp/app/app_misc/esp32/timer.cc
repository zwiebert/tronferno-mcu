/*
 * timer.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */

#include "app/config/proj_app_cfg.h"

#include "app/rtc.h"
#include "app/timer.h"
#include "app/settings/config.h"
#include "fernotron/trx/raw/fer_radio_timings.h"
#include "fernotron/trx/raw/fer_radio_trx.h"
#include "fernotron/int_timer.h"
#include "txtio/inout.h"

#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_types.h"
#include "hal/timer_types.h"
#include "misc/int_types.h"
#include "sdkconfig.h"
#include "soc/timer_group_struct.h"
#include <esp32/rom/ets_sys.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>




#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_INTERVAL0_SEC   (3.4179) // sample test interval for the first timer
#define TIMER_INTERVAL1_SEC   (5.78)   // sample test interval for the second timer
#define TEST_WITHOUT_RELOAD   0        // testing will be done without auto reload
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload
#define TIMER_SCALE_MS        (TIMER_SCALE / 1000)
#define TIMER_SCALE_US        (TIMER_SCALE_MS / 1000)



//////////////////////////////////////////////////////////////////////////
#ifndef USE_ESP_GET_TIME
volatile u32 run_time_s_, run_time_ts_;
#endif

static void IRAM_ATTR intTimer_isr(void *args) {
  int timer_idx = (int) args;

#ifdef FER_TRANSMITTER
  fer_tx_setOutput();
#endif
#ifdef FER_RECEIVER
  fer_rx_sampleInput();
#endif

  /* Retrieve the interrupt status and the counter value
   from the timer that reported the interrupt */
  TIMERG0.hw_timer[timer_idx].update = 1;
  /* Clear the interrupt
   and update the alarm time for the timer with without reload */
  TIMERG0.int_clr_timers.t1 = 1;
  /* After the alarm has been triggered
   we need enable it again, so it is triggered the next time */
  TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;

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

#ifndef USE_ESP_GET_TIME
  {
    static u32 s10_ticks = TICK_FREQ_HZ / 10;
    static int8_t s_ticks = 10;

    if (s10_ticks-- == 0) {
      s10_ticks = TICK_FREQ_HZ / 10;
      ++run_time_ts_;
      if (!s_ticks--) {
        s_ticks = 10;
        ++run_time_s_;
      }
    }
  }
#endif

}

static void intTimer_init(timer_group_t timer_group, timer_idx_t timer_idx, timer_autoreload_t auto_reload, unsigned interval_us) {

  timer_config_t config = { .alarm_en = TIMER_ALARM_EN,.counter_en = TIMER_PAUSE,  .intr_type =
      TIMER_INTR_LEVEL, .counter_dir = TIMER_COUNT_DOWN, .auto_reload = auto_reload, .divider = TIMER_DIVIDER};

  timer_init(timer_group, timer_idx, &config);
  timer_set_counter_value(timer_group, timer_idx, (1ULL * interval_us * TIMER_SCALE_US));
  timer_set_alarm_value(timer_group, timer_idx, 0);
  timer_enable_intr(timer_group, timer_idx);
  timer_isr_register(timer_group, timer_idx, intTimer_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
  timer_start(timer_group, timer_idx);
}

void intTimer_setup(void) {
  intTimer_init(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_EN, TICK_PERIOD_US);
}

