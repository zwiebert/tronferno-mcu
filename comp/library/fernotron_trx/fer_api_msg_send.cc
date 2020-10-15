#include <fernotron_trx/fer_trx_c_api.h>
#include "fernotron_trx/raw/fer_msg_rx.h"
#include <debug/dbg.h>


static void fill_fsb(fer_sbT &fsb, u32 a, u8 g, u8 m, fer_cmd cmd) {
  fer_init_sender(&fsb, a);

  if (FER_SB_ADDR_IS_CENTRAL(&fsb)) {
    FER_SB_PUT_NGRP(&fsb, g);
    FER_SB_PUT_NMEMB(&fsb, m);
  }
  FER_SB_PUT_CMD(&fsb, cmd);
}



bool fer_trx_send_cmd(const Fer_MsgCmd *msg) {
  fer_sbT fsb;
  fill_fsb(fsb,msg->a, msg->g, msg->m, (fer_cmd)msg->cmd);
  if (msg->stopDelay) {
    return fer_send_msg_with_stop(&fsb, msg->delay, msg->stopDelay, msg->repeats);
  } else {
    return fer_send_delayed_msg(&fsb, MSG_TYPE_PLAIN, msg->delay, msg->repeats);
  }

  return false;
}
bool fer_trx_send_rtc(const Fer_MsgRtc *msg) {
  fer_sbT fsb;
  fill_fsb(fsb,msg->a, msg->g, msg->m, fer_cmd_Program);

  return fer_send_rtc_message(&fsb, msg->rtc);

  return false;
}
bool fer_trx_send_timer(const Fer_MsgTimer *msg) {
  fer_sbT fsb;
  fill_fsb(fsb, msg->a, msg->g, msg->m, fer_cmd_Program);

  return fer_send_timer_message(&fsb, msg->rtc, msg->td);
}

bool fer_trx_send_empty_timer(const Fer_MsgRtc *msg) {
  fer_sbT fsb;
  fill_fsb(fsb, msg->a, msg->g, msg->m, fer_cmd_Program);

  return fer_send_empty_timer_message(&fsb, msg->rtc);

  return false;
}
