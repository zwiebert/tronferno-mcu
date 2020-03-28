#include <fernotron/fer_msg_plain.h>
#include <fernotron/fer_msg_rx.h>
#include "fernotron/fer_radio_trx.h"
#include <string.h>
#include "fer_app_cfg.h"
#include "fer_rawmsg_buffer.h"
#include "fer_msg_tx.h"
#include "debug/debug.h"
#include "app/timer.h"
#include "app/loop.h"
#include "misc/time/run_time.h"

bool fer_send_queued_msg(void);
void (*ferCb_beforeFirstSend)(const fsbT *fsb);
void (*ferCb_beforeAnySend)(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *fmsg);

u8 sf_toggle;
#define sf_SIZE 16
struct sf {
  fsbT fsb;
  u32 s10; // timer in 1 secs
  fmsg_type mt;
  i8 repeats;
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

#ifdef UNUSED_STATIC_FUNCTION
static fsbT *sf_shift() {
  if (sf_isEmpty())
    return 0;
  fsbT *fsb = &sf[sf_head].fsb;
  sf_incrHead();
  return fsb;
}
#endif

static bool   sf_append(const fsbT *fsb, fmsg_type msgType, u32 s10, u8 repeats) {
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
  sf[i].repeats = repeats;

  return false;
}

int ftx_get_msgPendingCount() { return (sf_tail_ + sf_SIZE - sf_head_) & (sf_SIZE - 1); }

static void fer_send_checkQuedState() {
  if (ftx_messageToSend_isReady)
    return;
  if (sf_isEmpty())
    return;

  ftx_READY_TO_TRANSMIT_cb();
}

bool 
ftx_is_ready(void) {
  if (ftx_messageToSend_isReady)
    return false;

  return true;
}

void
fer_tx_loop() {
  if (!ftx_messageToSend_isReady && !sf_isEmpty() && sf[sf_head].s10 <= get_now_time_ts()) {
#ifdef XXX_TOGGLE_STOP_REPEATS  // disabled because it blocks the receiver instead of making sure the stop is received //FIXME: not really true!?
    if (FSB_GET_CMD(&send_fsb[sf_head]) == fer_cmd_STOP) {
      fer_update_tglNibble(&send_fsb);
    }
#endif
    fer_send_queued_msg();
  }

  fer_send_checkQuedState();
}



bool  fer_send_msg_with_stop(const fsbT *fsb, u16 delay, u16 stopDelay, i8 repeats) {
  precond(fsb);
  precond(stopDelay > 0);

  if(fer_send_delayed_msg(fsb, MSG_TYPE_PLAIN, delay, repeats)) {
    fsbT stop_fsb = *fsb;
    FSB_PUT_CMD(&stop_fsb, fer_cmd_STOP);
    fer_update_tglNibble(&stop_fsb);
    return fer_send_delayed_msg(&stop_fsb, MSG_TYPE_PLAIN, delay + stopDelay, 2);
  }
  return false;
}

bool  fer_send_msg(const fsbT *fsb, fmsg_type msgType, i8 repeats) {
 //test// if (fsb) return fer_send_msg_with_stop(fsb, 5);
  return fer_send_delayed_msg(fsb, msgType, 0, repeats);
}

bool  fer_send_delayed_msg(const fsbT *fsb, fmsg_type msgType, u16 delay, i8 repeats) {
  precond(fsb);

  if (sf_isFull())
    return false;

  sf_append(fsb, msgType, delay + get_now_time_ts(), repeats);

  fer_send_checkQuedState();
  return true;
}



bool fer_send_queued_msg() {
  precond(!ftx_messageToSend_isReady);
  precond(!sf_isEmpty());

  const fsbT *fsb = &sf[sf_head].fsb;
  fmsg_type msgType = sf[sf_head].mt;

  static unsigned send_ct;

  if (send_ct++ == 0) {
    sf_toggle = fer_tglNibble_ctUp(sf_toggle, 1);
    FSB_PUT_TGL(&sf[sf_head].fsb, sf_toggle);
    if (msgType == MSG_TYPE_PLAIN) {
      if (ferCb_beforeFirstSend)
        ferCb_beforeFirstSend(fsb);
    }
  }

  if (--sf[sf_head].repeats < 0) {
    sf_incrHead();
    send_ct = 0;
  }

  memcpy(txmsg->cmd.bd, fsb->data, 5);
  fmsg_raw_checksumsCreate(txmsg, msgType);

  if (ferCb_beforeAnySend)
    ferCb_beforeAnySend(msgType, fsb, txmsg);

  ftx_messageToSend_wordCount = 2 * ((msgType == MSG_TYPE_PLAIN) ? BYTES_MSG_PLAIN : (msgType == MSG_TYPE_RTC) ? BYTES_MSG_RTC : BYTES_MSG_TIMER);

  ftx_messageToSend_isReady = true;

  return true;
}

fsbT *get_fsb(u32 a, u8 g, u8 m, fer_cmd cmd) {
  static fsbT fsb;
  fer_init_sender(&fsb, a);

  if (FSB_ADDR_IS_CENTRAL(&fsb)) {
    FSB_PUT_GRP(&fsb, g);
    FSB_PUT_MEMB(&fsb, m == 0 ? 0 : m + 7);
  }
  FSB_PUT_CMD(&fsb, cmd);

  return &fsb;
}

