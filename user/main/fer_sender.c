#include "common.h"
#include "fer.h"
#include "counter.h"
#include "inout.h"
#include "utils.h"
#include "config.h"
#include <string.h>

volatile bool is_sendMsgPending;
volatile uint16_t wordsToSend;

fer_sender_basic send_fsb;

bool ICACHE_FLASH_ATTR
fers_loop() {
  if (is_sendMsgPending)
    return false;

  if (send_fsb.repeats > 0) {
    if (FSB_GET_CMD(&send_fsb) == fer_cmd_STOP) {
      fer_update_tglNibble(&send_fsb);
    }
    fer_send_cmd(0);

  }
}

bool ICACHE_FLASH_ATTR fer_send_cmd(fer_sender_basic *fsb) {
  if (is_sendMsgPending)
    return false;
  if (fsb && send_fsb.repeats > 0)
    return false;
  if (recv_lockBuffer(true)) {
    if (fsb) {
      send_fsb = *fsb;
    } else {
      fsb = &send_fsb;
      --fsb->repeats;
    }

    memcpy(txmsg->cmd, fsb->data, 5);
    fmsg_create_checksums(txmsg, MSG_TYPE_PLAIN);

    if (C.app_verboseOutput >= vrb1)
      io_puts("S:"), fmsg_print(txmsg, MSG_TYPE_PLAIN);
    wordsToSend = 2 * BYTES_MSG_PLAIN;

    is_sendMsgPending = true;

    recv_lockBuffer(false);
    return true;
  } else {
    return false;
  }
}

bool ICACHE_FLASH_ATTR fer_send_prg(fer_sender_basic *fsb) {
  if (is_sendMsgPending)
    return false;
  memcpy(txmsg->cmd, fsb->data, 5);
  fmsg_create_checksums(txmsg, (FRB_GET_FPR0_IS_RTC_ONLY(txmsg->rtc) ? MSG_TYPE_RTC : MSG_TYPE_TIMER));

  if (C.app_verboseOutput >= vrb1)
    io_puts("S:"), fmsg_print(txmsg, C.app_verboseOutput >= vrb2 ? MSG_TYPE_TIMER : MSG_TYPE_PLAIN);

  wordsToSend = 2 * (FRB_GET_FPR0_IS_RTC_ONLY(txmsg->rtc) ? BYTES_MSG_RTC : BYTES_MSG_TIMER);
  is_sendMsgPending = true;
  return true;
}

