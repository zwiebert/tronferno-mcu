#include <fernotron_trx/raw/fer_msg_plain.h>
#include "fernotron_trx/fer_trx_c_api.h"
#include "fernotron_trx/raw/fer_radio_trx.h"
#include <string.h>
#include "fer_app_cfg.h"
#include "fernotron_trx/raw/fer_rawmsg_buffer.h"
#include "fernotron_trx/raw/fer_rawmsg_build.h"
#include "fernotron_trx/raw/fer_msg_tx.h"
#include "debug/dbg.h"

#include "fer_msg_tx_queue.h"
#include <fernotron_trx/fer_trx_api.hh>
#include "fer_trx_incoming_event.hh"
#include "fer_trx_impl.hh"
#include "utils_time/run_time.h"
#include "cc1101_ook/trx.hh"



void (*fer_tx_READY_TO_TRANSMIT_cb)(uint32_t when_to_transmit_ts);

static inline void fer_tx_ready_to_transmit_cb(uint32_t time_ts) {
  if (fer_tx_READY_TO_TRANSMIT_cb)
    fer_tx_READY_TO_TRANSMIT_cb(time_ts);
  fer_trx_api_pushEvent_readyToTransmit();
}

static void fer_send_checkQuedState() {
  struct sf *msg;

  if (fer_tx_isTransmitterBusy())
    return;

  if ((msg = fer_tx_nextMsg())) {
    fer_tx_ready_to_transmit_cb(msg->when_to_transmit_ts);
  }
}

bool fer_send_msg_with_stop(const fer_sbT *fsb, u16 delay, u16 stopDelay, i8 repeats) {
  precond(fsb);
  precond(stopDelay > 0);

  if (fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats, delay)) {
    fer_sbT stop_fsb = *fsb;
    FER_SB_PUT_CMD(&stop_fsb, fer_cmd_STOP);
    fer_update_tglNibble(&stop_fsb);
    return fer_send_msg(&stop_fsb, MSG_TYPE_PLAIN, 2, delay + stopDelay);
  }
  return false;
}

bool fer_send_msg(const fer_sbT *fsb, fer_msg_type msgType, i8 repeats, u16 delay) {
  precond(fsb);

  struct sf msg = { .fsb = *fsb, .when_to_transmit_ts = (delay + (u32)get_now_time_ts()), .mt = msgType, .repeats = repeats };
  if (!fer_tx_pushMsg(&msg))
    return false;

  fer_send_checkQuedState();
  return true;
}

static bool fer_send_queued_msg(struct sf *msg) {
  static u8 sf_toggle;
  precond(!fer_tx_isTransmitterBusy());
  precond(msg);

  Fer_Trx_IncomingEvent evt { .raw = fer_tx_msg, .fsb = msg->fsb, .kind = msg->mt, .tx = true };

  if (msg->sent_ct++ == 0) {
    sf_toggle = fer_tglNibble_ctUp(sf_toggle, 1);
    FER_SB_PUT_TGL(&msg->fsb, sf_toggle);
    if (msg->mt == MSG_TYPE_PLAIN) {
      evt.first = true;
      Fer_Trx_API::push_event(&evt);
    }
  }

  memcpy(fer_tx_msg->cmd.bd, msg->fsb.data, 5);
  fer_msg_raw_checksumsCreate(fer_tx_msg, msg->mt);

  evt.first = false;
  Fer_Trx_API::push_event(&evt);

  fer_tx_transmitFerMsg(0, msg->mt);

  if (msg->sent_ct > msg->repeats) {
    return true;
  }

  return false;
}

fer_sbT fer_construct_fsb(u32 a, u8 g, u8 m, fer_cmd cmd) {
  fer_sbT fsb;
  fer_init_sender(&fsb, a);

  if (FER_SB_ADDR_IS_CENTRAL(&fsb)) {
    FER_SB_PUT_NGRP(&fsb, g);
    FER_SB_PUT_NMEMB(&fsb, m);
  }
  FER_SB_PUT_CMD(&fsb, cmd);

  return fsb;
}

void fer_trx_direction(bool tx) {
  cc1101_ook_setDirection(tx);
}

void fer_tx_loop() {
  if (fer_tx_isTransmitterBusy())
    return;

  struct sf *msg = fer_tx_nextMsg();

  if (msg && msg->when_to_transmit_ts <= get_now_time_ts()) {
    // enable transmitter
    fer_trx_direction(true);
    if (fer_send_queued_msg(msg)) {
      fer_tx_popMsg();
    }
  } else {
    // enable receiver
    fer_trx_direction(false);
  }

  fer_send_checkQuedState();
}

