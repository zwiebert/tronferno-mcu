/*
 * fer_rx_msg.c
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */
#include "fer_app_cfg.h"
#include "fernotron/fer_msg_type.h"
#include "fernotron/fer_msg_rx.h"
#include "fer_rawmsg_buffer.h"
#include "timer_data.h"
#include "debug/debug.h"
#include <string.h>


void (*ferCb_rawMessageReceived)(fmsg_type msg_type, fsbT *fsb, fer_rawMsg *fmsg);

void (*ferCb_plainMessageReceived)(const fsbT *fsb);
void (*ferCb_rtcMessageReceived)(fer_rawMsg *fmsg);
void (*ferCb_timerMessageReceived)(timer_data_t *tdr);

fsbT last_received_sender;

static void notify_rawMessageReceived(fmsg_type msg_type) {
  if (ferCb_rawMessageReceived)
    ferCb_rawMessageReceived(msg_type, &last_received_sender, rxmsg);
  frx_clear();
}

static void notify_plainMessageReceived() {
  frx_clear();
  if (ferCb_plainMessageReceived) {
    ferCb_plainMessageReceived(&last_received_sender);
  }
}

static void notify_rtcMessageReceived() {
  if (!ferCb_rtcMessageReceived) {
    frx_clear();
    return;
  }
  assert(!"implemented"); // XXX
  frx_clear();
  //ferHook_rtcMessageReceived(xxx); //XXX
}

static void notify_timerMessageReceived() {
  if (!ferCb_timerMessageReceived) {
    frx_clear();
    return;
  }
  assert(!"implemented"); // XXX
  frx_clear();
  //ferHook_timerMessageReceived(xxx); //XXX
}

static void notify_messageReceived(fmsg_type msg_type) {
  switch (msg_type) {
  case MSG_TYPE_PLAIN:
    notify_plainMessageReceived();
    break;
  case MSG_TYPE_RTC:
    notify_rtcMessageReceived();
    break;
  case MSG_TYPE_TIMER:
    notify_timerMessageReceived();
    break;

  case MSG_TYPE_NONE:
  case MSG_TYPE_PLAIN_DOUBLE:
    break;
  }
}

bool fer_rx_loop() {
#ifdef FER_RECEIVER
  if (frx_messageReceived != MSG_TYPE_NONE) {
    fmsg_type msgType = frx_messageReceived;

    if (msgType == MSG_TYPE_PLAIN) {
      if (0 == memcmp(&last_received_sender.data, rxmsg->cmd.bd, 5)) {
        msgType = MSG_TYPE_PLAIN_DOUBLE;
      } else {
        memcpy(&last_received_sender.data, rxmsg->cmd.bd, 5);
      }
    }

    if (fmsg_raw_checksumsVerify(rxmsg, frx_messageReceived)) {
      notify_rawMessageReceived(msgType);
      notify_messageReceived(msgType);
    }
  }
#endif
  return true;
}
