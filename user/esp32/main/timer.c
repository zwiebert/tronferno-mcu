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


#if 0

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
#else

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_INTERVAL0_SEC   (3.4179) // sample test interval for the first timer
#define TIMER_INTERVAL1_SEC   (5.78)   // sample test interval for the second timer
#define TEST_WITHOUT_RELOAD   0        // testing will be done without auto reload
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload



/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
static void example_tgtimer0_timer_init(int timer_idx,
    bool auto_reload, double timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = auto_reload;
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer1_handler,
        (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);
}

void timer1_start() {
  example_tgtimer0_timer_init(TIMER_1, true, 0.000001 * TICK_PERIOD_US);
}

#endif
//////////////////////////////////////////////////////////////////////////

static void IRAM_ATTR timer1_handler(void *args) {
  int timer_idx = (int) args;

  /* Retrieve the interrupt status and the counter value
     from the timer that reported the interrupt */
  TIMERG0.hw_timer[timer_idx].update = 1;


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


  /* Clear the interrupt
     and update the alarm time for the timer with without reload */

  TIMERG0.int_clr_timers.t1 = 1;
    /* After the alarm has been triggered
    we need enable it again, so it is triggered the next time */
  TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;


}

void ICACHE_FLASH_ATTR setup_timer(void) {
  timer1_start();
}

