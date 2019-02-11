#include "common.h"
#include "fer.h"
#include "inout.h"
#include "config.h"
#include <string.h>

volatile bool is_sendMsgPending;
volatile uint16_t wordsToSend;

fer_sender_basic send_fsb;

bool ICACHE_FLASH_ATTR
fers_is_ready(void) {
  if (is_sendMsgPending)
    return false;

  return true;
}

bool ICACHE_FLASH_ATTR
fers_loop() {
  if (is_sendMsgPending)
    return false;

  if (send_fsb.repeats > 0) {
#ifdef XXX_TOGGLE_STOP_REPEATS  // disabled because it blocks the receiver instead of making sure the stop is received
    if (FSB_GET_CMD(&send_fsb) == fer_cmd_STOP) {
      fer_update_tglNibble(&send_fsb);
    }
#endif
    fer_send_msg(0, MSG_TYPE_PLAIN);

  }

  return true;
}


bool ICACHE_FLASH_ATTR fer_send_msg(fer_sender_basic *fsb, fmsg_type msgType) {
  if (is_sendMsgPending)
    return false;
#if 0
  if (fsb && send_fsb.repeats > 0)
    return false;
#else
  if (fsb && send_fsb.repeats > 0) {
    send_fsb.repeats = 0;
  }
#endif

  if (!recv_lockBuffer(true)) {
    return false;
  }

  if (msgType == MSG_TYPE_PLAIN) {
    if (fsb) {
      send_fsb = *fsb;
    } else {
      fsb = &send_fsb;
      --fsb->repeats;
    }
  }

  memcpy(txmsg->cmd, fsb->data, 5);
  fmsg_create_checksums(txmsg, msgType);

  if (C.app_verboseOutput >= vrb1)
    io_puts("S:"), fmsg_print(txmsg, C.app_verboseOutput >= vrb2 ? msgType : MSG_TYPE_PLAIN);

  wordsToSend = 2 * ((msgType == MSG_TYPE_PLAIN) ? BYTES_MSG_PLAIN
      : (msgType == MSG_TYPE_RTC) ? BYTES_MSG_RTC
          : BYTES_MSG_TIMER);


  is_sendMsgPending = true;
  recv_lockBuffer(false);

  return true;
}


