#include "app_config/proj_app_cfg.h"

#include "main.h"
#include <cli/cli.h>
#include <freertos/event_groups.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>
#include <cstdio>
#include <app_settings/config.h>
#include "utils_misc/int_types.h"
#include "utils_time/run_time.h"
#include "utils_time/ut_constants.hh"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron/pos/positions_dynamic.h"
#include "fernotron/pos/positions_static.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron_trx/fer_trx_c_api.h"
#include "net/ipnet.h"
#include "gpio/pin.h"

#include "../app_private.h"

EventGroupHandle_t loop_event_group;
#define EVT_BITS  ((1 << lf_Len) - 1)

uint32_t loop_flags_periodic_100ms;

typedef void (*lfa_funT)(void);

static const lfa_funT lfa_table[lf_Len] = {
#ifdef USE_NETWORK
    ipnet_connected,
    ipnet_disconnected,
#endif
#ifdef USE_NTP
    fer_am_updateTimerEvent,
#endif
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
    lfa_createWifiAp,
#endif
#ifdef USE_TCPS
    tcpCli_loop,
#endif
    cli_loop,
#ifdef FER_TRANSMITTER
    fer_tx_loop,
#endif
#ifdef FER_RECEIVER
    fer_rx_loop,
#endif
#ifdef USE_SEP
    fer_sep_loop,
#endif
    fer_pos_loop, fer_am_loop, fer_am_updateTimerEvent,
#ifdef USE_CUAS
    fer_cuas_set_check_timeout,
#endif
#ifdef USE_PAIRINGS
    fer_alias_auto_set_check_timeout,
#endif
    fer_statPos_loopAutoSave, fer_pos_loopCheckMoving,
    pin_notify_input_change,
    [] { mcu_delayedRestart(1500); }
};


void loop_eventBits_setup() {
  loop_event_group = xEventGroupCreate();
}
u32 loop_eventBits_wait() {
  EventBits_t bits = xEventGroupWaitBits(loop_event_group, EVT_BITS, pdTRUE, pdFALSE, portMAX_DELAY);
  return bits;
}

void loop_eventBits_check() {
  u32 lf = loop_eventBits_wait();
  for (int i = 0; lf; ++i, (lf >>= 1)) {
    auto fb = static_cast<loop_flagbits>(i);
    if (!GET_BIT(lf, 0))
      continue;

    if (lfa_table[fb])
      (lfa_table[fb])();
  }
}


#ifdef USE_WLAN_AP
void tmr_checkNetwork_start() {
  const int interval = pdMS_TO_TICKS(1000 * CHECK_NETWORK_INTERVAL);
  TimerHandle_t tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, nullptr, [](TimerHandle_t xTimer) {
    if (!ipnet_isConnected()) {
      lf_setBit(lf_createWifiAp);
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
            lf_setBit(lf_mcuRestart);  // XXX: restart every >=24 hours at 03:33
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
