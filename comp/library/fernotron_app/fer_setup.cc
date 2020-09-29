

#include "app/config/proj_app_cfg.h"

#include "debug/dbg.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/astro.h"
#include "fernotron/auto/fau_tevent.h"
#include "fernotron/cuas/cuid_auto_set.h"
#include "fernotron/trx/raw/fer_msg_attachment.h"
#include "fernotron/trx/raw/fer_msg_rx.h"
#include "fernotron/trx/raw/fer_msg_tx.h"
#include "fernotron/trx/raw/fer_radio_trx.h"
#include "fernotron/pos/shutter_pct.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/txtio/fer_print.h"
#include "fernotron/fer_main.h"
#include <fernotron/trx/fer_trx_api.hh>
#include <app/uout/callbacks.h>
#include <string.h>

struct fer_configT fer_config;
Fer_GmBitMask manual_bits;


static class IncomingMsg : public Fer_Trx_API {

public:
  virtual void event_plain_message_was_received() {
    Fer_MsgPlainCmd msg = get_msg();
    uoApp_publish_fer_msgReceived(&msg);

    if (is_centralUnit()) {
      fer_cuas_set_check(msg.a);
#ifdef USE_PAIRINGS
     fer_alias_auto_set_check(msg.a);
#endif
    }
    fer_simPos_registerMovingShutter(msg.a, msg.g, msg.m, msg.cmd);
  }
  //virtual void event_plain_double_message_was_received() {  }
  //virtual void event_rtc_message_was_received() {  }
  //virtual void event_timer_message_was_received() {  }
  virtual void event_any_message_was_received() {
    auto msg_type = get_msgKind();
    const fer_sbT *fsb = static_cast<const fer_sbT*>(get_fsb());
    const fer_rawMsg *fer_rx_msg = static_cast<const fer_rawMsg*>(get_raw());

    if (msg_type == MSG_TYPE_PLAIN || msg_type == MSG_TYPE_PLAIN_DOUBLE) {
      fer_msg_print("R:", fer_rx_msg, msg_type, TXTIO_IS_VERBOSE(vrbDebug));
     // fer_msg_print_as_cmdline((msg_type == MSG_TYPE_PLAIN_DOUBLE ? "Rc:" : "RC:"), fer_rx_msg, msg_type);
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

public:

  virtual void event_first_message_will_be_sent() { // no repeats
    const fer_sbT *fsb = static_cast<const fer_sbT*>(get_fsb());
    if (FER_SB_GET_DEVID(fsb) == fer_config.cu) {
      fer_simPos_registerMovingShutter(FER_SB_GET_DEVID(fsb), FER_SB_GET_GRP(fsb), FER_SB_GET_MEMB(fsb) == 0 ? 0 : FER_SB_GET_MEMB(fsb) - 7, (fer_if_cmd)FER_SB_GET_CMD(fsb));
    }
  }

  virtual void event_any_message_will_be_sent() { // first + repeats
    auto msg_type = get_msgKind();
    const fer_sbT *fsb = static_cast<const fer_sbT*>(get_fsb());
    const fer_rawMsg *fer_tx_msg = static_cast<const fer_rawMsg*>(get_raw());

    if (TXTIO_IS_VERBOSE(vrb1)) {
      auto t = TXTIO_IS_VERBOSE(vrb2) ? msg_type : fer_msg_kindT::MSG_TYPE_PLAIN;
      fer_msg_print("S:", fer_tx_msg, t, TXTIO_IS_VERBOSE(vrbDebug));
      fer_msg_print_as_cmdline("SC:", fer_tx_msg, t);
    }
  }

} MyIncomingMsg;




uint32_t fer_main_getSenderByAddress(long addr) {
  if (addr == 0) {
    return fer_config.cu;
  }
  return addr;
}





void fer_main_setup(const fer_configT &ferConfig, const bool reinit) {
   fer_config = ferConfig;
   manual_bits = Fer_GmBitMask("MANU");
   if (reinit)
     return;

  Fer_Trx_API::setup(&MyIncomingMsg);
  fer_pos_init();

}

