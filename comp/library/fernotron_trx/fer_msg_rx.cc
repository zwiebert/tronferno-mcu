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
#include "fernotron/trx/timer_data.h"
#include "debug/dbg.h"
#include <string.h>


void (*fer_rawMessageReceived_cb)(fer_msg_type msg_type, fer_sbT *fsb, fer_rawMsg *fmsg);

void (*fer_plainMessageReceived_cb)(const fer_sbT *fsb);
void (*fer_rtcMessageReceived_cb)(fer_rawMsg *fmsg);
void (*fer_timerMessageReceived_cb)(fer_rawMsg *fmsg);

fer_sbT last_received_sender;

static void notify_rawMessageReceived(fer_msg_type msg_type) {
  if (fer_rawMessageReceived_cb)
    fer_rawMessageReceived_cb(msg_type, &last_received_sender, fer_rx_msg);
}

static void notify_plainMessageReceived() {
  if (fer_plainMessageReceived_cb)
    fer_plainMessageReceived_cb(&last_received_sender);

}

static void notify_rtcMessageReceived() {
  if (fer_rtcMessageReceived_cb)
    fer_rtcMessageReceived_cb(fer_rx_msg);
}

static void notify_timerMessageReceived() {
  if (fer_timerMessageReceived_cb)
    fer_timerMessageReceived_cb(fer_rx_msg);
}

static void notify_messageReceived(fer_msg_type msg_type) {
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
  if (fer_rx_messageReceived != MSG_TYPE_NONE) {
    fer_msg_type msgType = fer_rx_messageReceived;

    if (msgType == MSG_TYPE_PLAIN) {
      if (0 == memcmp(&last_received_sender.data, fer_rx_msg->cmd.bd, 5)) {
        msgType = MSG_TYPE_PLAIN_DOUBLE;
      } else {
        memcpy(&last_received_sender.data, fer_rx_msg->cmd.bd, 5);
      }
    }

    if (fer_msg_raw_checksumsVerify(fer_rx_msg, fer_rx_messageReceived)) {
      notify_rawMessageReceived(msgType);
      notify_messageReceived(msgType);
      fer_rx_clear();
    }
  }
#endif
}
