#include <string.h>

#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/shutter_pct.h"
#include "app_config/proj_app_cfg.h"


#include "fernotron/auto/fau_tevent.h"
#include "cli/cli.h"
#include "app_settings/config.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "debug/dbg.h"
#include "app_misc/rtc.h"
#include "fernotron/alias/pairings.h"
#include "app_uout/status_output.h"
#include "fernotron/txtio/fer_print.h"
#include "fernotron_trx/astro.h"
#include "fernotron_trx/raw/fer_msg_attachment.h"
#include "fernotron_trx/raw/fer_msg_tx.h"
#include "fernotron_trx/raw/fer_msg_rx.h"
#include "fernotron_trx/callbacks.h"
#include "app_misc/fernotron.h"

void loop(void);
void tcpCli_loop(void);


#ifndef MCU_ESP32
void loop(void) {
  fer_tx_loop();

#ifdef USE_SEP
  fer_sep_loop();
#endif

  cli_loop();
  fer_pos_loop();

  fer_cuas_set_check_timeout();
#ifdef USE_PAIRINGS
  fer_alias_auto_set_check_timeout();
#endif

  fer_rx_loop();

#if defined USE_NTP && defined MCU_ESP8266
  bool  ntp_update_system_time(unsigned interval_seconds);
  if (ipnet_isConnected())
    ntp_update_system_time(SECS_PER_DAY);
#endif
}
#endif
