#include <string.h>

#include "fernotron_sep/set_endpos.h"
#include "fernotron_pos/shutter_pct.h"
#include "app/proj_app_cfg.h"


#include "fernotron_auto/timer_state.h"
#include "cli/cli.h"
#include "config/config.h"
#include "fernotron_cuas/cuid_auto_set.h"
#include "debug/debug.h"
#include "main/rtc.h"
#include "fernotron_alias/pairings.h"
#include "userio_app/status_output.h"
#include "fernotron_txtio/fer_print.h"
#include "fernotron/astro.h"
#include "fernotron/fer_msg_attachment.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/callbacks.h"
#include "app/fernotron.h"


void fer_copyConfig() {
  astro_cfg.geo_longitude = C.geo_longitude;
  astro_cfg.geo_latitude = C.geo_latitude;
  astro_cfg.geo_timezone = C.geo_timezone;
  astro_cfg.astroCorrection = C.astroCorrection;
}

static void rawMessageReceived_cb(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *rxmsg) {
  if (msg_type == MSG_TYPE_PLAIN || msg_type == MSG_TYPE_PLAIN_DOUBLE) {
    io_puts("R:"), fmsg_print(rxmsg, msg_type, (C.app_verboseOutput >= vrbDebug));
    io_puts((msg_type == MSG_TYPE_PLAIN_DOUBLE) ? "c:" : "C:"), fmsg_print_as_cmdline(rxmsg, msg_type);
  }

#ifndef FER_RECEIVER_MINIMAL

  if (msg_type == MSG_TYPE_RTC || msg_type == MSG_TYPE_TIMER) {
    io_puts("timer frame received\n"), fmsg_print(rxmsg, msg_type, (C.app_verboseOutput >= vrbDebug));
  }
#endif

}

static void plainMessageReceived_cb(const fsbT *fsb) {
  cu_auto_set_check(fsb);
#ifdef USE_PAIRINGS
  pair_auto_set_check(fsb);
#endif
  { //TODO: improve shutter states
    u8 g = 0, m = 0;

    if (FSB_ADDR_IS_CENTRAL(fsb)) {
      g = FSB_GET_GRP(fsb);
      m = FSB_GET_MEMB(fsb);
      if (m)
        m -= 7;
    }
    ferPos_registerMovingShutter(FSB_GET_DEVID(fsb), g, m, FSB_GET_CMD(fsb));
  }
}

static void beforeFirstSend_cb(const fsbT *fsb) {
 ferPos_registerMovingShutter(FSB_GET_DEVID(fsb), FSB_GET_GRP(fsb), FSB_GET_MEMB(fsb) == 0 ? 0 : FSB_GET_MEMB(fsb)-7, FSB_GET_CMD(fsb));
}

static void beforeAnySend_cb(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *txmsg) {
if (C.app_verboseOutput >= vrb1)
  io_puts("S:"), fmsg_print(txmsg, C.app_verboseOutput >= vrb2 ? msg_type : MSG_TYPE_PLAIN, (C.app_verboseOutput >= vrbDebug));
}

void loop(void) {


  fer_tx_loop();

#if ENABLE_SET_ENDPOS
  sep_loop();
#endif

  cli_loop();
  ferPos_loop();

  timer_state_loop();
  cu_auto_set_check_timeout();
#ifdef USE_PAIRINGS
  pair_auto_set_check_timeout();
#endif

  fer_rx_loop();

#if defined USE_NTP && defined MCU_ESP8266
  bool  ntp_update_system_time(unsigned interval_seconds);
  ntp_update_system_time(SECS_PER_DAY);
#endif
}


int 
main_setup() {

  ferCb_beforeFirstSend = beforeFirstSend_cb;
  ferCb_beforeAnySend = beforeAnySend_cb;
  ferCb_rawMessageReceived = rawMessageReceived_cb;
  ferCb_plainMessageReceived = plainMessageReceived_cb;

  rtc_setup();
  fer_init_sender(&default_sender, C.fer_centralUnitID);
  astro_init_and_reinit();

  so_output_message(SO_FW_START_MSG_PRINT, 0);
  ferPos_init();

  
  return 0;

}

