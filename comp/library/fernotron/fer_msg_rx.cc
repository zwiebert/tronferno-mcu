/*
 * fer_rx_msg.c
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */
#include "fer_app_cfg.h"
#include "fernotron/fer_msg_type.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "fernotron/timer_data.h"
#include "debug/dbg.h"
#include <string.h>


void (*fer_rawMessageReceived_cb)(fmsg_type msg_type, fsbT *fsb, fer_rawMsg *fmsg);

void (*fer_plainMessageReceived_cb)(const fsbT *fsb);
void (*fer_rtcMessageReceived_cb)(fer_rawMsg *fmsg);
void (*fer_timerMessageReceived_cb)(fer_rawMsg *fmsg);

fsbT last_received_sender;

static void notify_rawMessageReceived(fmsg_type msg_type) {
  if (fer_rawMessageReceived_cb)
    fer_rawMessageReceived_cb(msg_type, &last_received_sender, rxmsg);
}

static void notify_plainMessageReceived() {
  if (fer_plainMessageReceived_cb)
    fer_plainMessageReceived_cb(&last_received_sender);

}

static void notify_rtcMessageReceived() {
  if (fer_rtcMessageReceived_cb)
    fer_rtcMessageReceived_cb(rxmsg);
}

static void notify_timerMessageReceived() {
  if (fer_timerMessageReceived_cb)
    fer_timerMessageReceived_cb(rxmsg);
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

void fer_rx_loop() {
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
      frx_clear();
    }
  }
#endif
}
