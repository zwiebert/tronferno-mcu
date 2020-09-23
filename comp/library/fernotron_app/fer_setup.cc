

#include "app/config/proj_app_cfg.h"

#include "debug/dbg.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/astro.h"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron/callbacks.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "fernotron/fer_msg_attachment.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_radio_trx.h"
#include "fernotron/pos/shutter_pct.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/txtio/fer_print.h"
#include "fernotron/fer_main.h"

#include <string.h>


struct fer_configT fer_config;
fer_sbT default_sender;
GmBitMask manual_bits;

fer_sbT *get_sender_by_addr(long addr) {
  if (addr == 0) {
    return &default_sender;
  } else if (addr > 0 && 10 > addr) {
    //return &senders[addr];
  }
  return NULL ;
}


static void rawMessageReceived_cb(fer_msg_type msg_type, const fer_sbT *fsb, const fer_rawMsg *fer_rx_msg) {
  if (msg_type == MSG_TYPE_PLAIN || msg_type == MSG_TYPE_PLAIN_DOUBLE) {
    fer_msg_print("R:", fer_rx_msg, msg_type, TXTIO_IS_VERBOSE(vrbDebug));
    fer_msg_print_as_cmdline((msg_type == MSG_TYPE_PLAIN_DOUBLE ? "Rc:" : "RC:"), fer_rx_msg, msg_type);
  }

#ifndef FER_RECEIVER_MINIMAL
  if (msg_type == MSG_TYPE_RTC || msg_type == MSG_TYPE_TIMER) {
    fer_msg_print("timer frame received\n", fer_rx_msg, msg_type, TXTIO_IS_VERBOSE(vrbDebug));
  }
#endif

  if (TXTIO_IS_VERBOSE(vrbDebug)) {
    struct fer_rx_quality q;
    fer_rx_getQuality(&q);
    if (q.bad_pair_count)
      io_printf("RI:bad_word_pairs: %d\n", q.bad_pair_count);
  }
}

static void plainMessageReceived_cb(const fer_sbT *fsb) {
  cu_auto_set_check(fsb);
#ifdef USE_PAIRINGS
  pair_auto_set_check(fsb);
#endif
  {
    u8 g = 0, m = 0;

    if (FER_SB_ADDR_IS_CENTRAL(fsb)) {
      g = FER_SB_GET_GRP(fsb);
      m = FER_SB_GET_MEMB(fsb);
      if (m)
        m -= 7;
    }
    simPos_registerMovingShutter(FER_SB_GET_DEVID(fsb), g, m, FER_SB_GET_CMD(fsb));
  }
}

static void beforeFirstSend_cb(const fer_sbT *fsb) {
  if (FER_SB_GET_DEVID(fsb) == fer_config.cu) {
    simPos_registerMovingShutter(FER_SB_GET_DEVID(fsb), FER_SB_GET_GRP(fsb), FER_SB_GET_MEMB(fsb) == 0 ? 0 : FER_SB_GET_MEMB(fsb) - 7, FER_SB_GET_CMD(fsb));
  }
}

static void beforeAnySend_cb(fer_msg_type msg_type, const fer_sbT *fsb, const fer_rawMsg *fer_tx_msg) {
  if (TXTIO_IS_VERBOSE(vrb1)) {
    fer_msg_type t = TXTIO_IS_VERBOSE(vrb2) ? msg_type : MSG_TYPE_PLAIN;
    fer_msg_print("S:", fer_tx_msg, t, TXTIO_IS_VERBOSE(vrbDebug));
    fer_msg_print_as_cmdline("SC:", fer_tx_msg, t);
  }
}



void fer_setup(const fer_configT &ferConfig, const bool reinit) {
   fer_config = ferConfig;
   fer_init_sender(&default_sender, fer_config.cu);
   manual_bits = GmBitMask("MANU");
   if (reinit)
     return;

  fer_beforeFirstSend_cb = beforeFirstSend_cb;
  fer_beforeAnySend_cb = beforeAnySend_cb;
  fer_rawMessageReceived_cb = rawMessageReceived_cb;
  fer_plainMessageReceived_cb = plainMessageReceived_cb;
  ferPos_init();

}

