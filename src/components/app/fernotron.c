#include <string.h>

#include "fernotron_sep/set_endpos.h"
#include "fernotron_pos/shutter_pct.h"
#include "app_config/proj_app_cfg.h"


#include "fernotron_auto/timer_state_loop.h"
#include "cli/cli.h"
#include "config/config.h"
#include "fernotron_cuas/cuid_auto_set.h"
#include "debug/debug.h"
#include "app/rtc.h"
#include "fernotron_alias/pairings.h"
#include "userio_app/status_output.h"
#include "fernotron_txtio/fer_print.h"
#include "fernotron/astro.h"
#include "fernotron/fer_msg_attachment.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/callbacks.h"
#include "app/fernotron.h"

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
    simPos_registerMovingShutter(FSB_GET_DEVID(fsb), g, m, FSB_GET_CMD(fsb));
  }
}

static void beforeFirstSend_cb(const fsbT *fsb) {
 simPos_registerMovingShutter(FSB_GET_DEVID(fsb), FSB_GET_GRP(fsb), FSB_GET_MEMB(fsb) == 0 ? 0 : FSB_GET_MEMB(fsb)-7, FSB_GET_CMD(fsb));
}

static void beforeAnySend_cb(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *txmsg) {
  if (TXTIO_IS_VERBOSE(vrb1)) {
    fmsg_type t = TXTIO_IS_VERBOSE(vrb2) ? msg_type : MSG_TYPE_PLAIN;
    io_puts("S:"), fmsg_print(txmsg, t, TXTIO_IS_VERBOSE(vrbDebug));
    io_puts("C:"), fmsg_print_as_cmdline(txmsg, t);
  }
}



int 
main_setup() {

  ferCb_beforeFirstSend = beforeFirstSend_cb;
  ferCb_beforeAnySend = beforeAnySend_cb;
  ferCb_rawMessageReceived = rawMessageReceived_cb;
  ferCb_plainMessageReceived = plainMessageReceived_cb;

  rtc_setup();
  fer_init_sender(&default_sender, cfg_getCuId());
  astro_init_and_reinit(cfg_getAstro());

  so_output_message(SO_FW_START_MSG_PRINT, 0);
  ferPos_init();

  
  return 0;

}

