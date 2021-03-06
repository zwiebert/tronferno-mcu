/*
 * timer.c
 *
 *  Created on: 13.09.2017
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"

#include "app_misc/rtc.h"
#include "app_misc/timer.h"
#include "app_settings/config.h"
#include "fernotron_trx/isr_timer_config.h"
#include <fernotron_trx/fer_trx_api.hh>
#include "txtio/inout.h"

#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_types.h"
#include "esp_intr_alloc.h"
#include "hal/timer_types.h"
#include "utils_misc/int_types.h"
#include "sdkconfig.h"
#include "soc/timer_group_struct.h"
#include <esp32/rom/ets_sys.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define E1 0 // experimental: 2=enable pulse on TX-GPIO to show interrupt duration, 1=toggle tx each call of interrupt (useless)

constexpr unsigned TIMER_DIVIDER = 16; ///<   Hardware timer clock divider
constexpr unsigned TIMER_SCALE = (TIMER_BASE_CLK / TIMER_DIVIDER); ///<  convert counter value to seconds
constexpr unsigned TIMER_SCALE_MS = (TIMER_SCALE / 1000); ///<
constexpr unsigned TIMER_SCALE_US = (TIMER_SCALE_MS / 1000); ///<

//////////////////////////////////////////////////////////////////////////
#ifndef USE_ESP_GET_TIME
volatile u32 run_time_s_, run_time_ts_;
#endif

static void IRAM_ATTR intTimer_isr(void *args) {
  int timer_idx = (int) args;

#ifdef FER_TRANSMITTER
#if E1 == 1
  static bool e_tx_level;
  mcu_put_txPin (e_tx_level);
  e_tx_level = !e_tx_level;
#elif E1 == 2
  mcu_put_txPin(true);
#else
  mcu_put_txPin(Fer_Trx_API::isr_get_tx_level());
#endif
#endif
#ifdef FER_RECEIVER
  Fer_Trx_API::isr_sample_rx_pin(mcu_get_rxPin());
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

  static uint_fast8_t tick_count;
  ++tick_count;

#ifdef FER_TRANSMITTER
  {
    if (0 == (tick_count & ((FER_ISR_FMULT / FER_TX_FMULT) - 1))) {
      Fer_Trx_API::isr_handle_tx();
    }
  }
#endif
#ifdef FER_RECEIVER
  if (0 == (tick_count & ((FER_ISR_FMULT / FER_RX_FMULT) - 1))) {
    Fer_Trx_API::isr_handle_rx();
  }
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

#if E1 == 2
  mcu_put_txPin(false);
#endif
}

static void intTimer_init(timer_group_t timer_group, timer_idx_t timer_idx, timer_autoreload_t auto_reload, unsigned interval_us) {

  timer_config_t config = { .alarm_en = TIMER_ALARM_EN,.counter_en = TIMER_PAUSE,  .intr_type =
      TIMER_INTR_LEVEL, .counter_dir = TIMER_COUNT_DOWN, .auto_reload = auto_reload, .divider = TIMER_DIVIDER};

  timer_init(timer_group, timer_idx, &config);
  timer_set_counter_value(timer_group, timer_idx, (1ULL * interval_us * TIMER_SCALE_US));
  timer_set_alarm_value(timer_group, timer_idx, 0);
  timer_enable_intr(timer_group, timer_idx);
  timer_isr_register(timer_group, timer_idx, intTimer_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL3, NULL);
  timer_start(timer_group, timer_idx);
}

void app_timerISR_setup(void) {
  intTimer_init(TIMER_GROUP_0, TIMER_1, TIMER_AUTORELOAD_EN, FER_ISR_TICK_US);
}

