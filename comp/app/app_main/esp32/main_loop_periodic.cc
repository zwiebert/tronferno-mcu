/**
 * \file   app_main/esp32/main_loop_periodic.h
 * \brief  Run events in main task by setting event bits from periodic 100ms timer
 */
#include "app_config/proj_app_cfg.h"

#include "main_loop_periodic.h"

#include <utils_misc/int_types.h>
#include <utils_misc/int_macros.h>


#include "main.h"
#include <cli/cli.h>

#include <app_settings/config.h>
#include "utils_misc/int_macros.h"
#include "utils_misc/int_types.h"
#include "utils_time/run_time.h"
#include "utils_time/ut_constants.hh"
#include "main_loop/main_queue.hh"
#include "net/ipnet.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>

lfPerFlags mainLoop_PeriodicFlags;

void tmr_loopPeriodic100ms_start() {
  const int interval = pdMS_TO_TICKS(CONFIG_MAIN_LOOP_PERIOD_MS);
  TimerHandle_t tmr = xTimerCreate("PerLoop100ms", interval, pdTRUE, nullptr, [](TimerHandle_t xTimer) {
    static uint32_t count;
    ++count;


    mainLoop_callFun(lfPer100ms_mainFun);

    if ((count & (BIT(7) - 1)) == 0) { // 12,8 secs
      app_safeMode_increment(true);
    }

    if ((count & (BIT(9) - 1)) == 0) { // 51,2 secs
      if (run_time_s() > SECS_PER_DAY) {
        const time_t now = time(0);
        struct tm tms;
        if (auto tmp = localtime_r(&now, &tms)) {
          if (tmp->tm_hour == 3 && tmp->tm_min >= 33)
            mainLoop_mcuRestart(0);  // XXX: restart every >=24 hours at 03:33
        }
      }
    }
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PerLoopTimer start error");
  }
}
