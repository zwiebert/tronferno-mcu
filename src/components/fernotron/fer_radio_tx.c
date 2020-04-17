/*
 * fer_radio_tx.c
 *
 *  Created on: 16.04.2020
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"

#include "fer_radio_parity.h"

#include <fernotron/fer_msg_plain.h>
#include "int_timer.h"
#include "callbacks.h"
#include "fer_app_cfg.h"
#include "fer_rawmsg_buffer.h"
#include "fer_msg_tx.h"
#include "fernotron/extern.h"
#include "debug/debug.h"

struct ftrx_counter {
  u16 Words;
  u8 Ticks, Bits;
};

#ifdef FER_TRANSMITTER
/////////////////////////// transmitter /////////////////////////
static struct ftrx_counter ftxCount;
static bool output_level;   // output line
volatile u16 ftx_messageToSend_wordCount;
volatile bool ftx_messageToSend_isReady;
static fer_rawMsg *ftx_buf;

#undef bitLen
#define bitLen               FER_BIT_WIDTH_DCK
#define shortPositive_Len    FER_BIT_SHORT_DCK
#define longPositive_Len     FER_BIT_LONG_DCK
#define pre_Len              FER_PRE_WIDTH_DCK
#define pauseHigh_Len        FER_STP_NEDGE_DCK

void ftx_transmitFerMsg(fer_rawMsg *msg, fmsg_type msg_type) {
  precond(!ftx_isTransmitterBusy());
  ftx_buf = msg ? msg : txmsg;
  ftx_messageToSend_wordCount = ((msg_type == MSG_TYPE_PLAIN) ? WORDS_MSG_PLAIN : (msg_type == MSG_TYPE_RTC) ? WORDS_MSG_RTC : WORDS_MSG_TIMER);
  ftx_messageToSend_isReady = true;
}

/////////////////////////// interrupt code //////////////////////
bool IRAM_ATTR ftrx_testLoopBack_getRxPin() {
  return output_level;
}

#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)
#define init_counter() (ftxCount.Ticks = ftxCount.Bits = ftxCount.Words = 0)
#define advancePreCounter() (ct_incr(ftxCount.Ticks, pre_Len) && ct_incr(ftxCount.Bits, FER_PRE_BIT_CT))
#define advanceStopCounter() (ct_incr(ftxCount.Ticks, bitLen) && ct_incr(ftxCount.Bits, FER_STP_BIT_CT))
#define ftx_update_output_preamble() (output_level = (ftxCount.Ticks < shortPositive_Len))
#define ftx_update_output_stop() (output_level = (ftxCount.Ticks < pauseHigh_Len) && (ftxCount.Bits == 0))

// sets output line according to current bit in data word
// a stop bit is sent in ftxCount.Bits 0 .. 2
// a data word is sent in ftxCount.Bits 3 .. 10
static void IRAM_ATTR ftx_update_output_data(u16 word_buffer) {
  int bit = ftxCount.Bits - FER_STP_BIT_CT;

  if (bit < 0) {  // in stop bit (ftxCount.Bits 0 .. 2)
    output_level = ftxCount.Bits == 0 && ftxCount.Ticks < shortPositive_Len;
  } else { // in data word
    output_level = ftxCount.Ticks < shortPositive_Len || (ftxCount.Ticks < longPositive_Len && !(word_buffer & (1 << bit)));
  }
}

static bool IRAM_ATTR ftx_send_message() {
  static bool preamble_done, stop_done;
  static u16 word_buffer;

  if (!stop_done) {
    //send single stop bit before preamble
    ftx_update_output_stop();
    stop_done = advanceStopCounter();

  } else if (!preamble_done) {
    // send preamble
    ftx_update_output_preamble();
    if ((preamble_done = advancePreCounter())) {
      word_buffer = fer_add_word_parity(ftx_buf->cmd.bd[0], 0);
    }
  } else {
    // send data words + stop bits
    ftx_update_output_data(word_buffer);
    // counting ticks until end_of_frame
    if (ct_incr(ftxCount.Ticks, bitLen)) {
      // bit sent
      if ((ct_incr(ftxCount.Bits, FER_CMD_BIT_CT + FER_STP_BIT_CT))) {
        // word + stop sent
        if (ct_incr(ftxCount.Words, ftx_messageToSend_wordCount)) {
          // line sent
          stop_done = preamble_done = false;
          return true; // done
        } else {
          word_buffer = fer_add_word_parity(ftx_buf->cmd.bd[ftxCount.Words / 2], ftxCount.Words); // load next word
        }
      }
    }
  }
  return false; // continue
}

static void IRAM_ATTR ftx_tick_send_message() {

  bool done = ftx_send_message();
  mcu_put_txPin(output_level);

  if (done) {
    ftx_messageToSend_isReady = false;

    init_counter();
    mcu_put_txPin(0);

    ftx_MSG_TRANSMITTED_ISR_cb();
  }
}

void IRAM_ATTR ftx_tick(void) {

  if (ftx_messageToSend_isReady) {
    ftx_tick_send_message();
  }
}
#endif

