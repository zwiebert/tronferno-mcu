#include <fernotron/fer_msg_plain.h>
#include <fernotron/fer_msg_rx.h>
#include "fernotron/fer_radio_trx.h"
#include <string.h>
#include "fer_app_cfg.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "fernotron/fer_msg_tx.h"
#include "debug/dbg.h"
#include "misc/time/run_time.h"

#include "fer_msg_tx_queue.h"

void (*fer_beforeFirstSend_cb)(const fer_sbT *fsb);
void (*fer_beforeAnySend_cb)(fer_msg_type msg_type, const fer_sbT *fsb, const fer_rawMsg *fmsg);


void (*fer_tx_READY_TO_TRANSMIT_cb)(uint32_t time_ts);

static inline void fer_tx_ready_to_transmit_cb(uint32_t time_ts) {
  if (fer_tx_READY_TO_TRANSMIT_cb)
    fer_tx_READY_TO_TRANSMIT_cb(time_ts);
}

static void fer_send_checkQuedState() {
  struct sf *msg;

  if (fer_tx_isTransmitterBusy())
    return;

  if ((msg = fer_tx_nextMsg())) {
    fer_tx_ready_to_transmit_cb(msg->s10);
  }
}

bool fer_send_msg_with_stop(const fer_sbT *fsb, u16 delay, u16 stopDelay, i8 repeats) {
  precond(fsb);
  precond(stopDelay > 0);

  if (fer_send_delayed_msg(fsb, MSG_TYPE_PLAIN, delay, repeats)) {
    fer_sbT stop_fsb = *fsb;
    FER_SB_PUT_CMD(&stop_fsb, fer_cmd_STOP);
    fer_update_tglNibble(&stop_fsb);
    return fer_send_delayed_msg(&stop_fsb, MSG_TYPE_PLAIN, delay + stopDelay, 2);
  }
  return false;
}

bool fer_send_msg(const fer_sbT *fsb, fer_msg_type msgType, i8 repeats) {
  return fer_send_delayed_msg(fsb, msgType, 0, repeats);
}

bool fer_send_delayed_msg(const fer_sbT *fsb, fer_msg_type msgType, u16 delay, i8 repeats) {
  precond(fsb);

  struct sf msg = { .fsb = *fsb, .s10 = (delay + (u32)get_now_time_ts()), .mt = msgType, .repeats = repeats };
  if (!fer_tx_pushMsg(&msg))
    return false;

  fer_send_checkQuedState();
  return true;
}

static bool fer_send_queued_msg(struct sf *msg) {
  static u8 sf_toggle;

  precond(!fer_tx_isTransmitterBusy());
  precond(msg);

  if (msg->sent_ct++ == 0) {
    sf_toggle = fer_tglNibble_ctUp(sf_toggle, 1);
    FER_SB_PUT_TGL(&msg->fsb, sf_toggle);
    if (msg->mt == MSG_TYPE_PLAIN) {
      if (fer_beforeFirstSend_cb)
        fer_beforeFirstSend_cb(&msg->fsb);
    }
  }

  memcpy(fer_tx_msg->cmd.bd, msg->fsb.data, 5);
  fer_msg_raw_checksumsCreate(fer_tx_msg, msg->mt);

  if (fer_beforeAnySend_cb)
    fer_beforeAnySend_cb(msg->mt, &msg->fsb, fer_tx_msg);

  fer_tx_transmitFerMsg(0, msg->mt);

  if (msg->sent_ct > msg->repeats) {
    return true;
  }

  return false;
}

fer_sbT* fer_get_fsb(u32 a, u8 g, u8 m, fer_cmd cmd) {
  static fer_sbT fsb;
  fer_init_sender(&fsb, a);

  if (FER_SB_ADDR_IS_CENTRAL(&fsb)) {
    FER_SB_PUT_NGRP(&fsb, g);
    FER_SB_PUT_NMEMB(&fsb, m);
  }
  FER_SB_PUT_CMD(&fsb, cmd);

  return &fsb;
}

void fer_tx_loop() {
  if (fer_tx_isTransmitterBusy())
    return;

  struct sf *msg = fer_tx_nextMsg();

  if (msg && msg->s10 <= get_now_time_ts()) {
    if (fer_send_queued_msg(msg)) {
      fer_tx_popMsg();
    }
  }

  fer_send_checkQuedState();
}

