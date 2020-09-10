#include <string.h>

#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/shutter_pct.h"
#include "app_config/proj_app_cfg.h"


#include "fernotron/auto/fau_tevent.h"
#include "cli/cli.h"
#include "config/config.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "debug/dbg.h"
#include "app/rtc.h"
#include "fernotron/alias/pairings.h"
#include "uout_app/status_output.h"
#include "fernotron/txtio/fer_print.h"
#include "fernotron/astro.h"
#include "fernotron/fer_msg_attachment.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/callbacks.h"
#include "app/fernotron.h"

void loop(void);
void tcpCli_loop(void);


#ifndef MCU_ESP32
void loop(void) {
  fer_tx_loop();

#ifdef USE_SEP
  sep_loop();
#endif

  cli_loop();
  ferPos_loop();

  fam_loop_old();
  cu_auto_set_check_timeout();
#ifdef USE_PAIRINGS
  pair_auto_set_check_timeout();
#endif

  fer_rx_loop();

#if defined USE_NTP && defined MCU_ESP8266
  bool  ntp_update_system_time(unsigned interval_seconds);
  if (ipnet_isConnected())
    ntp_update_system_time(SECS_PER_DAY);
#endif
}
#endif
