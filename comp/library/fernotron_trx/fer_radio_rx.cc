#include "app/config/proj_app_cfg.h"

#include "fer_radio_parity.h"
#include <fernotron/trx/raw/fer_msg_plain.h>
#include "fernotron/int_timer.h"
#include <stdlib.h>
#include "fernotron/trx/callbacks.h"
#include "fer_app_cfg.h"
#include "fernotron/trx/raw/fer_rawmsg_buffer.h"
#include "fernotron/trx/raw/fer_msg_tx.h"
#include "fernotron/extern.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"


//  the same timings relative to ticks of interrupt frequency
#define FER_PRE_WIDTH_TCK       DATA_CLOCK_TO_TICKS(FER_PRE_WIDTH_DCK)
#define FER_PRE_WIDTH_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_WIDTH_MIN_DCK)
#define FER_PRE_WIDTH_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_WIDTH_MAX_DCK)
#define FER_PRE_NEDGE_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_NEDGE_MIN_DCK)
#define FER_PRE_NEDGE_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_NEDGE_MAX_DCK)

#define FER_STP_WIDTH_TCK       DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_DCK)
#define FER_STP_WIDTH_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MIN_DCK)
#define FER_STP_WIDTH_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MAX_DCK)
#define FER_STP_NEDGE_TCK       DATA_CLOCK_TO_TICKS(FER_STP_NEDGE_DCK)
#define FER_STP_NEDGE_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_STP_NEDGE_MIN_DCK)
#define FER_STP_NEDGE_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_STP_NEDGE_MAX_DCK)

#define FER_BIT_WIDTH_TCK       DATA_CLOCK_TO_TICKS(FER_BIT_WIDTH_DCK)
#define FER_BIT_SHORT_TCK       DATA_CLOCK_TO_TICKS(FER_BIT_SHORT_DCK)
#define FER_BIT_LONG_TCK        DATA_CLOCK_TO_TICKS(FER_BIT_LONG_DCK)
#define FER_BIT_SAMP_POS_TCK    DATA_CLOCK_TO_TICKS(FER_BIT_SAMP_POS_DCK)

#ifndef DISTRIBUTION
//#define FTRX_TEST_LOOP_BACK
#endif

#ifdef FTRX_TEST_LOOP_BACK
bool ftrx_testLoopBack_getRxPin();
#define mcu_get_rxPin ftrx_testLoopBack_getRxPin
#endif

#if defined FER_RECEIVER

void (*fer_rx_MSG_RECEIVED_ISR_cb)(void);

static inline void fer_rx_msg_received_isr_cb() {
  if (fer_rx_MSG_RECEIVED_ISR_cb)
    fer_rx_MSG_RECEIVED_ISR_cb();
}


struct fer_rx_counter {
  u16 Words;
  u8 Bits, preBits, stopBits;
  u8 errors; u8 recovered;
};
static bool input_level, input_edge_pos, input_edge_neg;
static u16 aTicks, pTicks, nTicks;
static u16 wordsToReceive;
// flags
volatile fer_msg_type fer_rx_messageReceived;
static u16 word_pair_buffer[2];
static struct fer_rx_counter frxCount;


void fer_rx_getQuality(struct fer_rx_quality *dst) {
  dst->bad_pair_count = frxCount.recovered;
}

/////////////////////////// interrupt code //////////////////////
#define bitLen               DATA_CLOCK_TO_TICKS(FER_BIT_WIDTH_DCK)
// bit is 1, if data edge comes before sample position (/SHORT\..long../)
// bit is 0, if data edge comes after sample position  (/..LONG..\short/)
#define SAMPLE_BIT ((pTicks < DATA_CLOCK_TO_TICKS(FER_BIT_SAMP_POS_DCK)))
#define veryLongPauseLow_Len (2 * DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MAX_DCK))
#define MAX_PRG_TICK_COUNT (DATA_CLOCK_TO_TICKS(FER_PRG_FRAME_WIDTH_DCK) * 2) // FIXME
#define rxbuf_current_byte() (&fer_rx_msg->cmd.bd[0] + (frxCount.Words / 2))
#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)


static fer_error IRAM_ATTR fer_rx_extract_Byte(const u16 *src, u8 *dst) {
  bool match = ((0xff & src[0]) == (0xff & src[1]));
  unsigned count = 0;
  u8 out_byte = 0x77;

  if (fer_word_parity_p(src[0], 0)) {
    out_byte = src[0];
    ++count;

  }

  if (fer_word_parity_p(src[1], 1)) {
    out_byte = src[1];
    ++count;
  }

  *dst = out_byte;

  if (count == 2 && match) {
    return fer_OK;
  } else if (count == 1) {
    return fer_PAIR_NOT_EQUAL;
  }
  return fer_BAD_WORD_PARITY;
}

static fer_error  IRAM_ATTR fer_rx_verify_cmd(const u8 *dg) {
  int i;
  u8 checksum = 0;
  bool all_null = true;

  for (i = 0; i < FER_CMD_BYTE_CT - 1; ++i) {
    checksum += dg[i];
    if (dg[i] != 0)
      all_null = false;
  }

  if (all_null)
    return fer_BAD_CHECKSUM;

  return (checksum == dg[i] ? fer_OK : fer_BAD_CHECKSUM);
}

static void IRAM_ATTR fer_rx_recv_decodeByte(u8 *dst) {
  switch (fer_rx_extract_Byte(word_pair_buffer, dst)) {
  case fer_PAIR_NOT_EQUAL:
    ++frxCount.recovered;
    break;
  case fer_BAD_WORD_PARITY:
    ++frxCount.errors;
    break;
  case fer_OK:
  default:
    break;
  }
}

static bool  IRAM_ATTR fer_rx_is_stopBit(unsigned len, unsigned nedge) {
  return ((FER_STP_WIDTH_MIN_TCK <= len && len <= FER_STP_WIDTH_MAX_TCK) && (FER_STP_NEDGE_MIN_TCK <= nedge && nedge <= FER_STP_NEDGE_MAX_TCK));
}

static bool  IRAM_ATTR fer_rx_is_pre_bit(unsigned len, unsigned nedge) {
  return ((FER_PRE_WIDTH_MIN_TCK <= len && len <= FER_PRE_WIDTH_MAX_TCK) && (FER_PRE_NEDGE_MIN_TCK <= nedge && nedge <= FER_PRE_NEDGE_MAX_TCK));
}


static bool IRAM_ATTR fer_rx_wait_and_sample(void) {
  if (!input_edge_pos)
    return false;

  if (frxCount.preBits < 5) {
    if (fer_rx_is_pre_bit(aTicks, pTicks))
      ++frxCount.preBits;

    return false;
  }


  if (frxCount.stopBits < 1) {
    if (fer_rx_is_stopBit(aTicks, pTicks))
      ++frxCount.stopBits;
    if (frxCount.stopBits == 0 && frxCount.Words)
      fer_rx_clear();
    return false;
  }


  PUT_BIT(word_pair_buffer[frxCount.Words & 1], frxCount.Bits, pTicks < nTicks);
  return true;
}


static int IRAM_ATTR fer_rx_receive_message(void) {

  if (fer_rx_wait_and_sample()) {
    if (ct_incr(frxCount.Bits, FER_CMD_BIT_CT)) {
      frxCount.stopBits = 0;
      // word complete
      if ((frxCount.Words & 1) == 1) {
        // word pair complete
        fer_rx_recv_decodeByte(rxbuf_current_byte());
      }
      return ++frxCount.Words;
    }
  }

  return 0;  // continue
}

void IRAM_ATTR  fer_rx_clear(void) {
  frxCount = (struct fer_rx_counter) {};
  fer_rx_messageReceived = MSG_TYPE_NONE;
  wordsToReceive = WORDS_MSG_PLAIN;
}

static void IRAM_ATTR fer_rx_tick_receive_message() {

  switch (fer_rx_receive_message()) {

  case WORDS_MSG_PLAIN:
    if (frxCount.errors || fer_OK != fer_rx_verify_cmd(fer_rx_msg->cmd.bd)) {
      fer_rx_clear();
      break;
    }

    if (fer_rx_msg->cmd.sd.cmd.cmd == fer_cmd_Program && FER_RB_ADDR_IS_CENTRAL(fer_rx_msg->cmd.bd)) {
      wordsToReceive = WORDS_MSG_RTC; // continue
    } else {
      fer_rx_messageReceived = MSG_TYPE_PLAIN; // done
    }
    break;

  case WORDS_MSG_RTC:
    if (frxCount.errors) {
      fer_rx_clear();
      break;
    }

    if (FER_RB_GET_FPR0_IS_RTC_ONLY(fer_rx_msg->rtc.bd)) {
      fer_rx_messageReceived = MSG_TYPE_RTC;  // done
    } else {
      wordsToReceive = WORDS_MSG_TIMER; // continue
    }
    break;

  case WORDS_MSG_TIMER:
    if (frxCount.errors) {
      fer_rx_clear();
      break;
    }

    fer_rx_messageReceived = MSG_TYPE_TIMER; // done
    break;

  default:
    break; // continue
  }
}

void IRAM_ATTR fer_rx_sampleInput() {
  bool pin_level = mcu_get_rxPin();

  input_edge_pos = input_edge_neg = false;
  if (input_level != pin_level) {
    if (pin_level)
      input_edge_pos = true;
    else
      input_edge_neg = true;
  }

  input_level = pin_level;
}

void IRAM_ATTR fer_rx_tick() {

  if ((input_level && nTicks > veryLongPauseLow_Len)) {
    fer_rx_clear();
  }

  // receive and decode input
  if (!fer_rx_isReceiverBlocked()) {
    fer_rx_tick_receive_message();
  }

  if (input_edge_pos)
    aTicks = pTicks = 0;
  if (input_edge_neg)
    nTicks = 0;

  // measure the time between input edges
  ++aTicks;
  if (input_level) {
    ++pTicks;
  } else {
    ++nTicks;
  }

  if (fer_rx_messageReceived != MSG_TYPE_NONE) {
    fer_rx_msg_received_isr_cb();
  }
}

#endif

