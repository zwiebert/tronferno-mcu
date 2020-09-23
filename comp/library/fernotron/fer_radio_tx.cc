/*
 * fer_radio_tx.c
 *
 *  Created on: 16.04.2020
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"

#include "fer_radio_parity.h"

#include <fernotron/fer_msg_plain.h>
#include "fernotron/int_timer.h"
#include "fernotron/callbacks.h"
#include "fer_app_cfg.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/extern.h"
#include "debug/dbg.h"

struct ftrx_counter {
  u16 Words;
  u16 Ticks, Bits;
};

#ifdef FER_TRANSMITTER
/////////////////////////// transmitter /////////////////////////
static struct ftrx_counter ftxCount;
static bool output_level;   // output line
volatile u16 fer_tx_messageToSend_wordCount;
volatile bool fer_tx_messageToSend_isReady;
static fer_rawMsg *fer_tx_buf;

void (*fer_tx_MSG_TRANSMITTED_ISR_cb)(void);



void fer_tx_transmitFerMsg(fer_rawMsg *msg, fer_msg_type msg_type) {
  precond(!fer_tx_isTransmitterBusy());
  fer_tx_buf = msg ? msg : fer_tx_msg;
  fer_tx_messageToSend_wordCount = ((msg_type == MSG_TYPE_PLAIN) ? WORDS_MSG_PLAIN : (msg_type == MSG_TYPE_RTC) ? WORDS_MSG_RTC : WORDS_MSG_TIMER);
  fer_tx_messageToSend_isReady = true;
}

/////////////////////////// interrupt code //////////////////////
#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)
#define init_counter() (ftxCount.Ticks = ftxCount.Bits = ftxCount.Words = 0)


#define advanceLeadIntCounter() (ct_incr(ftxCount.Ticks, FER_INIT_WIDTH_DCK))
#define fer_tx_update_output_lead_in() (output_level = (ftxCount.Ticks < FER_INIT_NEDGE_DCK))

#define advanceStopBitCounter() (ct_incr(ftxCount.Ticks, FER_STP_WIDTH_DCK))
#define fer_tx_update_output_stop_bit() (output_level = (ftxCount.Ticks < FER_STP_NEDGE_DCK))

#define advancePreambleCounter() (ct_incr(ftxCount.Ticks, FER_PRE_WIDTH_DCK) && ct_incr(ftxCount.Bits, FER_PRE_BIT_CT))
#define fer_tx_update_output_preamble() (output_level = (ftxCount.Ticks < FER_PRE_NEDGE_DCK))

#define advanceWordBitCounter() (ct_incr(ftxCount.Ticks, FER_BIT_WIDTH_DCK) && ct_incr(ftxCount.Bits, FER_CMD_BIT_CT))
#define fer_tx_update_output_data(word_buffer) (output_level = ftxCount.Ticks < FER_BIT_SHORT_DCK || (ftxCount.Ticks < FER_BIT_LONG_DCK && !GET_BIT(word_buffer, ftxCount.Bits)))

#define advanceWordCounter() (ct_incr(ftxCount.Words, fer_tx_messageToSend_wordCount))

#define advanceLeadOutCounter() (ct_incr(ftxCount.Ticks, 18)) // XXX: add pause after done (usefulness?)

static inline void IRAM_ATTR fer_tx_msg_transmitted_isr_cb() {
  if (fer_tx_MSG_TRANSMITTED_ISR_cb)
    fer_tx_MSG_TRANSMITTED_ISR_cb();
}

#if 0 // compiler bug
// XXX: this crashes because somehow the switch() statement uses inaccessible memory (0x3f40_xxxx) for jumping to case labels
// XXX: workaround: GCC option -fno-jump-tables or using if/else
#pragma GCC push_options
#pragma GCC optimize ("O0")

static bool IRAM_ATTR fer_tx_send_message() {
  static enum {    state_lead_in, state_preamble, state_data_stop_bit, state_data_word, state_lead_out,  } fer_tx_state;
  static u16 word_buffer;

  switch (fer_tx_state) {
  case state_lead_in:
    fer_tx_update_output_lead_in();
    if (advanceLeadIntCounter())
      fer_tx_state = state_preamble;
    break;

  case state_preamble:
    fer_tx_update_output_preamble();
    if (advancePreambleCounter())
      fer_tx_state = state_data_stop_bit;
    break;

  case state_data_stop_bit:
    fer_tx_update_output_stop_bit();
    if (advanceStopBitCounter()) {
      fer_tx_state = state_data_word;
      word_buffer = fer_add_word_parity(fer_tx_buf->cmd.bd[ftxCount.Words / 2], ftxCount.Words);
    }
    break;

  case state_data_word:
    fer_tx_update_output_data(word_buffer);
    if (advanceWordBitCounter())
      fer_tx_state = advanceWordCounter() ? state_lead_out : state_data_stop_bit;
    break;

  case state_lead_out:
    output_level = 0;
    if (advanceLeadOutCounter()) {
      fer_tx_state = state_lead_in;
      return true;
    }

  }

return false; // continue
}
#pragma GCC pop_options

#else

static bool IRAM_ATTR fer_tx_send_message() {
  static enum {
    state_lead_in, state_preamble, state_data_stop_bit, state_data_word, state_lead_out,
  } fer_tx_state;
  static u16 word_buffer;

  if (fer_tx_state == state_lead_in) {
    fer_tx_update_output_lead_in();
    if (advanceLeadIntCounter())
      fer_tx_state = state_preamble;

  } else if (fer_tx_state == state_preamble) {
    fer_tx_update_output_preamble();
    if (advancePreambleCounter())
      fer_tx_state = state_data_stop_bit;

  } else if (fer_tx_state == state_data_stop_bit) {
    fer_tx_update_output_stop_bit();
    if (advanceStopBitCounter()) {
      fer_tx_state = state_data_word;
      word_buffer = fer_add_word_parity(fer_tx_buf->cmd.bd[ftxCount.Words / 2], ftxCount.Words);
    }

  } else if (fer_tx_state == state_data_word) {
    fer_tx_update_output_data(word_buffer);
    if (advanceWordBitCounter())
      fer_tx_state = advanceWordCounter() ? state_lead_out : state_data_stop_bit;

  } else if (fer_tx_state == state_lead_out) {
    output_level = 0;
    if (advanceLeadOutCounter()) {
      fer_tx_state = state_lead_in;
      return true;
    }

  }

return false; // continue
}
#endif

static void IRAM_ATTR fer_tx_dck_send_message() {

  bool done = fer_tx_send_message();
  if (done) {
    fer_tx_messageToSend_isReady = false;
    init_counter();
    output_level = false;
    fer_tx_msg_transmitted_isr_cb();
  }
}


void IRAM_ATTR fer_tx_setOutput(void) {
  mcu_put_txPin(output_level);
}

void IRAM_ATTR fer_tx_dck(void) {
  if (fer_tx_messageToSend_isReady) {
    fer_tx_dck_send_message();
  }
}

// debug function
bool IRAM_ATTR ftrx_testLoopBack_getRxPin() {
  return output_level;
}

#endif

