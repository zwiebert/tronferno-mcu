/*
 * fer_radio_tx.c
 *
 *  Created on: 16.04.2020
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"

#include <fernotron/fer_msg_plain.h>
#include "int_timer.h"
#include <stdlib.h>
#include "callbacks.h"
#include "fer_app_cfg.h"
#include "fer_rawmsg_buffer.h"
#include "fer_msg_tx.h"
#include "fernotron/extern.h"
#include "app/loop.h"
#include "debug/debug.h"

struct ftrx_counter {
  u16 Words;
  u8 Ticks, Bits;
};

#ifdef FER_TRANSMITTER
/////////////////////////// transmitter /////////////////////////
static u16 dtSendBuf;
static struct ftrx_counter ftxCount;
static bool tx_output;   // output line
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
  precond (!ftx_isTransmitterBusy());
  ftx_buf = msg ? msg : txmsg;
  ftx_messageToSend_wordCount = 2 * ((msg_type == MSG_TYPE_PLAIN) ? BYTES_MSG_PLAIN : (msg_type == MSG_TYPE_RTC) ? BYTES_MSG_RTC : BYTES_MSG_TIMER);
  ftx_messageToSend_isReady = true;
}

/////////////////////////// interrupt code //////////////////////
bool IRAM_ATTR ftrx_testLoopBack_getRxPin() {
  return tx_output;
}

#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)
#define make_Word fer_add_word_parity
#define init_counter() (ftxCount.Ticks = ftxCount.Bits = ftxCount.Words = 0)
#define advancePreCounter() (ct_incr(ftxCount.Ticks, pre_Len) && ct_incr(ftxCount.Bits, FER_PRE_BIT_CT))
#define advanceStopCounter() (ct_incr(ftxCount.Ticks, bitLen) && ct_incr(ftxCount.Bits, FER_STP_BIT_CT))

#define ftx_update_output_preamble() (tx_output = (ftxCount.Ticks < shortPositive_Len))
#define ftx_update_output_stop() (tx_output = (ftxCount.Ticks < pauseHigh_Len) && (ftxCount.Bits == 0))

/*  "t if VAL contains an even number of 1 bits" */
static bool IRAM_ATTR is_bits_even(u8 val) {
  val ^= val >> 4;
  val ^= val >> 2;
  val ^= val >> 1;
  val &= 0x01;
  return (val == 0);
}

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
static u8 IRAM_ATTR fer_get_word_parity(u8 data_byte, u8 pos) {
  u8 result;
  bool is_even = is_bits_even(data_byte);

  result = ((pos & 1)) ? (is_even ? 3 : 1) : (is_even ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
static u16  IRAM_ATTR fer_add_word_parity(u8 data_byte, int pos) {
  u16 result = (data_byte | (((u16) fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

// sets output line according to current bit in data word
// a stop bit is sent in ftxCount.Bits 0 .. 2
// a data word is sent in ftxCount.Bits 3 .. 10
static void  IRAM_ATTR ftx_update_output_data() {
  int bit = ftxCount.Bits - FER_STP_BIT_CT;

  if (bit < 0) {  // in stop bit (ftxCount.Bits 0 .. 2)
    tx_output = ftxCount.Bits == 0 && ftxCount.Ticks < shortPositive_Len;
  } else { // in data word
    tx_output = ftxCount.Ticks < shortPositive_Len || (ftxCount.Ticks < longPositive_Len && !(dtSendBuf & (1 << bit)));
  }
}

static bool  IRAM_ATTR ftx_send_message() {
  static bool preamble_done, stop_done;

  if (!stop_done) {
    //send single stop bit before preamble
    ftx_update_output_stop();
    stop_done = advanceStopCounter();

  } else if (!preamble_done) {
    // send preamble
    ftx_update_output_preamble();
    if ((preamble_done = advancePreCounter())) {
      dtSendBuf = make_Word(ftx_buf->cmd.bd[0], 0);
    }
  } else {
    // send data words + stop bits
    ftx_update_output_data();
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
          dtSendBuf = make_Word(ftx_buf->cmd.bd[ftxCount.Words / 2], ftxCount.Words); // load next word
        }
      }
    }
  }
  return false; // continue
}

static void  IRAM_ATTR ftx_tick_send_message() {

  bool done = ftx_send_message();
  mcu_put_txPin(tx_output);

  if (done) {
    ftx_messageToSend_isReady = false;

    init_counter();
    mcu_put_txPin(0);

    ftx_MSG_TRANSMITTED_ISR_cb();
  }
}

void IRAM_ATTR  ftx_tick(void) {

  if (ftx_messageToSend_isReady) {
    ftx_tick_send_message();
  }
}
#endif

