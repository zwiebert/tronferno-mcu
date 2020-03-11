/*
 * fer_rx_msg.c
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */
#include "fer_app_cfg.h"
#include "fernotron/fer_msg.h"
#include "fer.h"
#include "timer_data.h"
#include "debug/debug.h"

void (*ferHook_rawMessageReceived)(fmsg_type msg_type, fsbT *fsb, fer_msg *fmsg);

void (*ferHook_plainMessageReceived)(const fsbT *fsb);
void (*ferHook_rtcMessageReceived)(fer_msg *fmsg);
void (*ferHook_timerMessageReceived)(timer_data_t *tdr);

fsbT last_received_sender;

static void callHook_rawMessageReceived(fmsg_type msg_type) {
  if (ferHook_rawMessageReceived)
    ferHook_rawMessageReceived(msg_type, &last_received_sender, rxmsg);
  frx_clear();
}

static void callHook_plainMessageReceived() {
  frx_clear();
  if (ferHook_plainMessageReceived) {
    ferHook_plainMessageReceived(&last_received_sender);
  }
}

static void callHook_rtcMessageReceived() {
  if (!ferHook_rtcMessageReceived) {
    frx_clear();
    return;
  }
  assert(!"implemented"); // XXX
  frx_clear();
  //ferHook_rtcMessageReceived(xxx); //XXX
}

static void callHook_timerMessageReceived() {
  if (!ferHook_timerMessageReceived) {
    frx_clear();
    return;
  }
  assert(!"implemented"); // XXX
  frx_clear();
  //ferHook_timerMessageReceived(xxx); //XXX
}

static void callHooks_messageReceived(fmsg_type msg_type) {
  switch (msg_type) {
  case MSG_TYPE_PLAIN:
    callHook_plainMessageReceived();
    break;
  case MSG_TYPE_RTC:
    callHook_rtcMessageReceived();
    break;
  case MSG_TYPE_TIMER:
    callHook_timerMessageReceived();
    break;

  case MSG_TYPE_NONE:
  case MSG_TYPE_PLAIN_DOUBLE:
    break;
  }
}

bool fer_rx_loop() {
#ifdef FER_RECEIVER
  if (MessageReceived != MSG_TYPE_NONE) {
    fmsg_type msgType = MessageReceived;

    if (msgType == MSG_TYPE_PLAIN) {
      if (0 == memcmp(&last_received_sender.data, rxmsg->cmd.bd, 5)) {
        msgType = MSG_TYPE_PLAIN_DOUBLE;
      } else {
        memcpy(&last_received_sender.data, rxmsg->cmd.bd, 5);
      }
    }

    if (fmsg_verify_checksums(rxmsg, MessageReceived)) {
      callHook_rawMessageReceived(msgType);
      callHooks_messageReceived(msgType);
    }
  }
#endif
  return true;
}
