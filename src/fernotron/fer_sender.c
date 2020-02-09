#include <string.h>
#include "user_config.h"

#include "fernotron/fer.h"
#include "config/config.h"
#include "userio/inout.h"
#include "main/rtc.h"
#include "fernotron/fer_code.h"

volatile bool is_sendMsgPending;
volatile u16 wordsToSend;

u8 sf_toggle;
#define sf_SIZE 8
struct {
  fer_sender_basic fsb;
  u32 s10; // timer in 1 secs
} sf[sf_SIZE];

u8 sf_head_, sf_tail_;
#define sf_head (sf_head_+0)
#define sf_tail (sf_tail_+0)
#define sf_incrHead() ((void)(sf_head_ = ((sf_head+1) & (sf_SIZE-1))))
#define sf_incrTail() ((void)(sf_tail_ = ((sf_tail+1) & (sf_SIZE-1))))
#define sf_isEmpty() (sf_head_ == sf_tail)
#define sf_isFull()  (sf_head_ == (((sf_tail+1) & (sf_SIZE-1))))
static fer_sender_basic *sf_shift() {
  if (sf_isEmpty())
    return 0;
  fer_sender_basic *fsb = &sf[sf_head].fsb;
  sf_incrHead();
  return fsb;
}
static bool sf_append(const fer_sender_basic *fsb, u32 s10) {
  if (sf_isFull())
    return false;
  sf[sf_tail].fsb = *fsb;
  sf[sf_tail].s10 = s10;
  sf_toggle = fer_tglNibble_ctUp(sf_toggle, 1);
  FSB_PUT_TGL(&sf[sf_tail].fsb, sf_toggle);
  sf_incrTail();
  return false;
}

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

  if (!sf_isEmpty() && sf[sf_head].s10 <= run_time(0)) {
#ifdef XXX_TOGGLE_STOP_REPEATS  // disabled because it blocks the receiver instead of making sure the stop is received //FIXME: not really true!?
    if (FSB_GET_CMD(&send_fsb[sf_head]) == fer_cmd_STOP) {
      fer_update_tglNibble(&send_fsb);
    }
#endif
    fer_send_msg(0, MSG_TYPE_PLAIN);

  }

  return true;
}

bool ICACHE_FLASH_ATTR fer_send_msg_with_stop(const fer_sender_basic *fsb, u16 delay, u16 stopDelay) {
  precond(fsb);
  precond(stopDelay > 0);

  if(fer_send_delayed_msg(fsb, MSG_TYPE_PLAIN, delay)) {
    fer_sender_basic stop_fsb = *fsb;
    FSB_PUT_CMD(&stop_fsb, fer_cmd_STOP);
    stop_fsb.repeats = 2;
    fer_update_tglNibble(&stop_fsb);
    return fer_send_delayed_msg(&stop_fsb, MSG_TYPE_PLAIN, delay + stopDelay);
  }
  return false;
}

bool ICACHE_FLASH_ATTR fer_send_msg(const fer_sender_basic *fsb, fmsg_type msgType) {
 //test// if (fsb) return fer_send_msg_with_stop(fsb, 5);
  return fer_send_delayed_msg(fsb, msgType, 0);
}

bool ICACHE_FLASH_ATTR fer_send_delayed_msg(const fer_sender_basic *fsb, fmsg_type msgType, u16 delay) {
  if (is_sendMsgPending)
    return false;

  if (!recv_lockBuffer(true)) {
    return false;
  }

  if (fsb) {

  }

  if (msgType == MSG_TYPE_PLAIN) {
    if (fsb) {
      if (sf_isFull()) {
        return false;
      } else {
        sf_append(fsb, delay + run_time(0));
        return true;
      }
    } else {
      if (sf_isEmpty()) {
        return false;
      } else {
        --sf[sf_head].fsb.repeats;
        fsb = &sf[sf_head].fsb;
        if (fsb->repeats < 0) {
          sf_incrHead();
        }
      }
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

