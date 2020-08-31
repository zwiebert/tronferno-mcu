#include <fernotron/fer_msg_plain.h>
#include <fernotron/fer_msg_rx.h>
#include "fernotron/fer_radio_trx.h"
#include <string.h>
#include "fer_app_cfg.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "fernotron/fer_msg_tx.h"
#include "debug/debug.h"
#include "app/timer.h"
#include "app/loop.h"
#include "misc/time/run_time.h"

#include "fer_msg_tx_queue.h"

void (*ferCb_beforeFirstSend)(const fsbT *fsb);
void (*ferCb_beforeAnySend)(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *fmsg);

static void fer_send_checkQuedState() {
  struct sf *msg;

  if (ftx_isTransmitterBusy())
    return;

  if ((msg = ftx_nextMsg())) {
    ftx_READY_TO_TRANSMIT_cb(msg->s10);
  }
}

bool fer_send_msg_with_stop(const fsbT *fsb, u16 delay, u16 stopDelay, i8 repeats) {
  precond(fsb);
  precond(stopDelay > 0);

  if (fer_send_delayed_msg(fsb, MSG_TYPE_PLAIN, delay, repeats)) {
    fsbT stop_fsb = *fsb;
    FSB_PUT_CMD(&stop_fsb, fer_cmd_STOP);
    fer_update_tglNibble(&stop_fsb);
    return fer_send_delayed_msg(&stop_fsb, MSG_TYPE_PLAIN, delay + stopDelay, 2);
  }
  return false;
}

bool fer_send_msg(const fsbT *fsb, fmsg_type msgType, i8 repeats) {
  return fer_send_delayed_msg(fsb, msgType, 0, repeats);
}

bool fer_send_delayed_msg(const fsbT *fsb, fmsg_type msgType, u16 delay, i8 repeats) {
  precond(fsb);

  struct sf msg = { .fsb = *fsb, .s10 = (delay + (u32)get_now_time_ts()), .mt = msgType, .repeats = repeats };
  if (!ftx_pushMsg(&msg))
    return false;

  fer_send_checkQuedState();
  return true;
}

static bool fer_send_queued_msg(struct sf *msg) {
  static u8 sf_toggle;

  precond(!ftx_isTransmitterBusy());
  precond(msg);

  if (msg->sent_ct++ == 0) {
    sf_toggle = fer_tglNibble_ctUp(sf_toggle, 1);
    FSB_PUT_TGL(&msg->fsb, sf_toggle);
    if (msg->mt == MSG_TYPE_PLAIN) {
      if (ferCb_beforeFirstSend)
        ferCb_beforeFirstSend(&msg->fsb);
    }
  }

  memcpy(txmsg->cmd.bd, msg->fsb.data, 5);
  fmsg_raw_checksumsCreate(txmsg, msg->mt);

  if (ferCb_beforeAnySend)
    ferCb_beforeAnySend(msg->mt, &msg->fsb, txmsg);

  ftx_transmitFerMsg(0, msg->mt);

  if (msg->sent_ct > msg->repeats) {
    return true;
  }

  return false;
}

fsbT* get_fsb(u32 a, u8 g, u8 m, fer_cmd cmd) {
  static fsbT fsb;
  fer_init_sender(&fsb, a);

  if (FSB_ADDR_IS_CENTRAL(&fsb)) {
    FSB_PUT_NGRP(&fsb, g);
    FSB_PUT_NMEMB(&fsb, m);
  }
  FSB_PUT_CMD(&fsb, cmd);

  return &fsb;
}

void fer_tx_loop() {
  if (ftx_isTransmitterBusy())
    return;

  struct sf *msg = ftx_nextMsg();

  if (msg && msg->s10 <= get_now_time_ts()) {
    if (fer_send_queued_msg(msg)) {
      ftx_popMsg();
    }
  }

  fer_send_checkQuedState();
}

