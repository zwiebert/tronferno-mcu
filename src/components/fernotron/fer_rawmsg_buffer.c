/*
 * fer_rawmsg_buffer.c
 *
 *  Created on: 19.03.2020
 *      Author: bertw
 */

#include "fernotron/fer_msg_attachment.h"
#include "fer_app_cfg.h"
#include "fer_rawmsg_buffer.h"
#include "int_timer.h"
#include "stdbool.h"
#include "main/common.h"

struct fer_raw_msg message_buffer;

#ifndef FER_RECEIVER
bool  ftrx_lockBuffer(bool enableLock) {
  return true;
}
#else
volatile u8 msgBuf_requestLock;
volatile bool msgBuf_isLocked;

bool ftrx_lockBuffer(bool enableLock) {
  if (enableLock) {
    msgBuf_requestLock++;
    do {
      mcu_delay_us(100);
    } while (!msgBuf_isLocked);
  } else {
    msgBuf_requestLock--;
  }
  return true;
}
#endif
