#include "app_config/proj_app_cfg.h"

#include "main.h"
#include <cli/cli.h>
#include <freertos/event_groups.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>
#include <cstdio>
#include <app_settings/config.h>
#include "utils_misc/int_macros.h"
#include "utils_misc/int_types.h"
#include "utils_time/run_time.h"
#include "utils_time/ut_constants.hh"
#include "main_loop/main_queue.hh"
#include "net/ipnet.h"


#include "../app_private.h"


uint32_t loop_flags_periodic_100ms;

typedef void (*lfa_funT)(void);


#ifdef USE_WLAN_AP
void tmr_checkNetwork_start() {
  const int interval = pdMS_TO_TICKS(1000 * CHECK_NETWORK_INTERVAL);
  TimerHandle_t tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, nullptr, [](TimerHandle_t xTimer) {
    if (!ipnet_isConnected()) {
      mainLoop_callFun(lfa_createWifiAp);
    }
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}
#endif

/**
 * \brief Interval for periodic events. Will also restart MCU periodically to avoid memory fragmentation.
 * \param[in] loop_flags_periodic_100ms  global variable with loop_flagbits
 */
void tmr_loopPeriodic100ms_start() {
  const int interval = pdMS_TO_TICKS(LOOP_PERIODIC_INTERVAL_MS);
  TimerHandle_t tmr = xTimerCreate("PerLoop100ms", interval, pdTRUE, nullptr, [](TimerHandle_t xTimer) {
    static uint32_t count;
    ++count;
    if (loop_flags_periodic_100ms)
      lf_setBits(loop_flags_periodic_100ms);

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

void loop(void) {
  loop_eventBits_check();
}
