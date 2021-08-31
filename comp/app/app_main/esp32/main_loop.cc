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




void loop(void) {
  mainLoop_processMessages(0,5000);
}
