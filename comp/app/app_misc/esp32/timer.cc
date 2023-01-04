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
#include "utils_misc/int_types.h"

#include "sdkconfig.h"

#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define E1 0 // experimental: 2=enable pulse on TX-GPIO to show interrupt frequency and duration, 1=toggle tx each call of interrupt (useless)

//////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_APP_USE_ESP_GET_TIME
volatile u32 run_time_s_, run_time_ts_;
#endif

static void IRAM_ATTR intTimer_isr_work() {
  static uint_fast8_t tick_count;
  ++tick_count;
  const bool isTxTick = (0 == (tick_count & ((FER_ISR_FMULT / FER_TX_FMULT) - 1)));
  const bool isRxTick = (0 == (tick_count & ((FER_ISR_FMULT / FER_RX_FMULT) - 1)));

#ifdef CONFIG_APP_USE_FER_TRANSMITTER
#if E1 == 1
  static bool e_tx_level;
  mcu_put_txPin (e_tx_level);
  e_tx_level = !e_tx_level;
#elif E1 == 2
  mcu_put_txPin(true);
#else
  if (isTxTick)
    mcu_put_txPin(Fer_Trx_API::isr_get_tx_level());
#endif
#endif
#ifdef CONFIG_APP_USE_FER_RECEIVER
  bool rx_pin_lvl = mcu_get_rxPin();
#endif

#ifdef CONFIG_APP_USE_FER_TRANSMITTER
  {
    if (isTxTick) {
      Fer_Trx_API::isr_handle_tx();
    }
  }
#endif
#ifdef CONFIG_APP_USE_FER_RECEIVER
  if (isRxTick) {
    Fer_Trx_API::isr_handle_rx(rx_pin_lvl);
  }
#endif

#ifndef CONFIG_APP_USE_ESP_GET_TIME
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

static bool IRAM_ATTR intTimer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
  intTimer_isr_work();
  return false;
}

static void intTimer_init() {
  gptimer_handle_t gptimer = NULL;
  gptimer_config_t timer_config = { .clk_src = GPTIMER_CLK_SRC_DEFAULT, .direction = GPTIMER_COUNT_UP, .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
      };
  ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

  gptimer_alarm_config_t config = { .alarm_count = FER_ISR_TICK_US, .reload_count = 0, .flags = { .auto_reload_on_alarm = 1 } };

  ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &config));
  gptimer_event_callbacks_t cbs = { .on_alarm = intTimer_isr };
  ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
  ESP_ERROR_CHECK(gptimer_enable(gptimer));
  ESP_ERROR_CHECK(gptimer_start(gptimer));
}

void app_timerISR_setup(void) {
  intTimer_init();
}

