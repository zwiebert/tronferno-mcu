#include <string.h>

#include "../shutter_positions/current_state.h"
#include "user_config.h"

#include "fernotron/fer.h"
#include "config/config.h"
#include "userio/inout.h"
#include "main/rtc.h"
#include "fernotron/fer_code.h"

volatile bool is_sendMsgPending;
volatile u16 wordsToSend;

bool fer_send_queued_msg(void);

extern volatile u32 run_time_s10;
#define run_time_10(x) (run_time_s10 + 0)

u8 sf_toggle;
#define sf_SIZE 8
struct sf {
  fer_sender_basic fsb;
  u32 s10; // timer in 1 secs
  fmsg_type mt;
} sf[sf_SIZE];

u8 sf_head_, sf_tail_;
#define sf_head (sf_head_+0)
#define sf_tail (sf_tail_+0)
#define sf_incrHead() ((void)(sf_head_ = ((sf_head+1) & (sf_SIZE-1))))
#define sf_incrTail() ((void)(sf_tail_ = ((sf_tail+1) & (sf_SIZE-1))))
#define sf_Pred(a) ((((u8)(a))-1) & (sf_SIZE-1))
#define sf_Succ(a) ((((u8)(a))+1) & (sf_SIZE-1))
#define sf_isEmpty() (sf_head_ == sf_tail)
#define sf_isFull()  (sf_head_ == (((sf_tail+1) & (sf_SIZE-1))))
static fer_sender_basic *sf_shift() {
  if (sf_isEmpty())
    return 0;
  fer_sender_basic *fsb = &sf[sf_head].fsb;
  sf_incrHead();
  return fsb;
}

static bool   sf_append(const fer_sender_basic *fsb, fmsg_type msgType, u32 s10) {
  if (sf_isFull())
    return false;
  u8 i = sf_tail;
  u8 k = sf_Pred(i);
  sf_incrTail();

  while(i != sf_head)  {
    if (s10 >= sf[k].s10) {
      break;
    }
    sf[i] = sf[k];
    i = k;
    k = sf_Pred(i);
  }

  sf[i].fsb = *fsb;
  sf[i].s10 = s10;
  sf[i].mt = msgType;

  return false;
}

bool 
fers_is_ready(void) {
  if (is_sendMsgPending)
    return false;

  return true;
}

bool 
fers_loop() {
  if (is_sendMsgPending)
    return false;

  if (!sf_isEmpty() && sf[sf_head].s10 <= run_time_10(0)) {
#ifdef XXX_TOGGLE_STOP_REPEATS  // disabled because it blocks the receiver instead of making sure the stop is received //FIXME: not really true!?
    if (FSB_GET_CMD(&send_fsb[sf_head]) == fer_cmd_STOP) {
      fer_update_tglNibble(&send_fsb);
    }
#endif
    fer_send_queued_msg();
  }

  return true;
}

bool  fer_send_msg_with_stop(const fer_sender_basic *fsb, u16 delay, u16 stopDelay) {
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

bool  fer_send_msg(const fer_sender_basic *fsb, fmsg_type msgType) {
 //test// if (fsb) return fer_send_msg_with_stop(fsb, 5);
  return fer_send_delayed_msg(fsb, msgType, 0);
}

bool  fer_send_delayed_msg(const fer_sender_basic *fsb, fmsg_type msgType, u16 delay) {
  precond(fsb);

  if (sf_isFull())
    return false;

  sf_append(fsb, msgType, delay + run_time_10(0));
  return true;

}

bool  fer_send_queued_msg() {
  if (is_sendMsgPending)
    return false;
  if (sf_isEmpty())
    return false;

  if (!recv_lockBuffer(true)) {
    return false;
  }

  const fer_sender_basic *fsb = &sf[sf_head].fsb;
  fmsg_type msgType = sf[sf_head].mt;

  static unsigned send_ct;

  if (send_ct == 0) {
    sf_toggle = fer_tglNibble_ctUp(sf_toggle, 1);
    FSB_PUT_TGL(&sf[sf_head].fsb, sf_toggle);
    if (msgType == MSG_TYPE_PLAIN) {
      currentState_Move(FSB_GET_DEVID(fsb), FSB_GET_GRP(fsb), FSB_GET_MEMB(fsb) == 0 ? 0 : FSB_GET_MEMB(fsb)-7, FSB_GET_CMD(fsb));
    }

  }

  --sf[sf_head].fsb.repeats;
  ++send_ct;

  if (fsb->repeats < 0) {
    sf_incrHead();
    send_ct = 0;
  }

  memcpy(txmsg->cmd, fsb->data, 5);
  fmsg_create_checksums(txmsg, msgType);

  if (C.app_verboseOutput >= vrb1)
    io_puts("S:"), fmsg_print(txmsg, C.app_verboseOutput >= vrb2 ? msgType : MSG_TYPE_PLAIN);

  wordsToSend = 2 * ((msgType == MSG_TYPE_PLAIN) ? BYTES_MSG_PLAIN : (msgType == MSG_TYPE_RTC) ? BYTES_MSG_RTC : BYTES_MSG_TIMER);

  is_sendMsgPending = true;
  recv_lockBuffer(false);

  return true;
}

