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


struct frx_counter {
  u16 Words;
  u8 Bits, preBits, stopBits;
  u8 errors;
};

/////////////////////////// interrupt code //////////////////////
#if defined FER_RECEIVER
// buffer to store received RF data
static u16 dtRecvBuffer[2];
static struct frx_counter frxCount;

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



#define bitLen               DATA_CLOCK_TO_TICKS(FER_BIT_WIDTH_DCK)
// bit is 1, if data edge comes before sample position (/SHORT\..long../)
// bit is 0, if data edge comes after sample position  (/..LONG..\short/)
#define SAMPLE_BIT ((pTicks < DATA_CLOCK_TO_TICKS(FER_BIT_SAMP_POS_DCK)))

#define IS_P_INPUT (rx_input == true)
#define IS_N_INPUT (rx_input == false)
#define veryLongPauseLow_Len (2 * DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MAX_DCK))

#define pEdge (input_edge > 0)
#define nEdge (input_edge < 0)

#define MAX_PRG_TICK_COUNT (DATA_CLOCK_TO_TICKS(FER_PRG_FRAME_WIDTH_DCK) * 2) // FIXME

#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)

static i8 input_edge; // Usually 0, but -1 or +1 at the tick where input has changed
static u16 aTicks, pTicks, nTicks;

// holds sampled value of input pin
static bool rx_input;

// flags
volatile u8 frx_messageReceived;

#define rxbuf_current_byte() (&rxmsg->cmd.bd[0] + (frxCount.Words / 2))
static u16 wordsToReceive;


/* "return t if parity is even and position matches parity bit \ 1/3
 on even positions and 0,2 on odd positions)" */
static bool  IRAM_ATTR fer_word_parity_p(u16 word, u8 pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}

static fer_error  IRAM_ATTR frx_extract_Byte(const u16 *src, u8 *dst) {
#if 0
  if (fer_word_parity_p(src[0], 0)
      && fer_word_parity_p(src[1], 1)
      && ((0xff & src[0]) == (0xff & src[1]))) {
    *dst = src[0];
    return fer_OK;
  }

#else
  if (fer_word_parity_p(src[0], 0)) {
    *dst = src[0];
    return fer_OK;
  } else if (fer_word_parity_p(src[1], 1)) {
    *dst = src[1];
    return fer_OK;
  }
#endif

  *dst = 0x77;  // error
  return fer_BAD_WORD_PARITY;
}

static fer_error  IRAM_ATTR frx_verify_cmd(const u8 *dg) {
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

static void  IRAM_ATTR frx_recv_decodeByte(u8 *dst) {
  if (fer_OK != frx_extract_Byte(dtRecvBuffer, dst)) {
    ++frxCount.errors;
  }
}

static bool  IRAM_ATTR frx_is_stopBit(unsigned len, unsigned nedge) {
  return ((FER_STP_WIDTH_MIN_TCK <= len && len <= FER_STP_WIDTH_MAX_TCK) && (FER_STP_NEDGE_MIN_TCK <= nedge && nedge <= FER_STP_NEDGE_MAX_TCK));
}

static bool  IRAM_ATTR frx_is_pre_bit(unsigned len, unsigned nedge) {
  return ((FER_PRE_WIDTH_MIN_TCK <= len && len <= FER_PRE_WIDTH_MAX_TCK) && (FER_PRE_NEDGE_MIN_TCK <= nedge && nedge <= FER_PRE_NEDGE_MAX_TCK));
}


static bool IRAM_ATTR frx_wait_and_sample(void) {
  if (!pEdge)
    return false;

  if (frxCount.preBits < 5) {
    if (frx_is_pre_bit(aTicks, pTicks))
      ++frxCount.preBits;

    return false;
  }


  if (frxCount.stopBits < 1) {
    if (frx_is_stopBit(aTicks, pTicks))
      ++frxCount.stopBits;
    if (frxCount.stopBits == 0 && frxCount.Words)
      frx_clear();
    return false;
  }


  PUT_BIT(dtRecvBuffer[frxCount.Words & 1], frxCount.Bits, pTicks < nTicks);
  return true;
}


static int IRAM_ATTR frx_receive_message(void) {

  if (frx_wait_and_sample()) {
    if (ct_incr(frxCount.Bits, FER_CMD_BIT_CT)) {
      frxCount.stopBits = 0;
      // word complete
      if ((frxCount.Words & 1) == 1) {
        // word pair complete
        frx_recv_decodeByte(rxbuf_current_byte());
      }
      return ++frxCount.Words;
    }
  }

  return 0;  // continue
}

void IRAM_ATTR  frx_clear(void) {
  frxCount = (struct frx_counter) {};
  frx_messageReceived = 0;
  wordsToReceive = WORDS_MSG_PLAIN;
}

static void IRAM_ATTR frx_tick_receive_message() {

  switch (frx_receive_message()) {

  case WORDS_MSG_PLAIN:
    if (frxCount.errors || fer_OK != frx_verify_cmd(rxmsg->cmd.bd)) {
      frx_clear();
      break;
    }

    if (rxmsg->cmd.sd.cmd.cmd == fer_cmd_Program && FRB_ADDR_IS_CENTRAL(rxmsg->cmd.bd)) {
      wordsToReceive = WORDS_MSG_RTC; // continue
    } else {
      frx_messageReceived = MSG_TYPE_PLAIN; // done
    }
    break;

  case WORDS_MSG_RTC:
    if (frxCount.errors) {
      frx_clear();
      break;
    }

    if (FRB_GET_FPR0_IS_RTC_ONLY(rxmsg->rtc.bd)) {
      frx_messageReceived = MSG_TYPE_RTC;  // done
    } else {
      wordsToReceive = WORDS_MSG_TIMER; // continue
    }
    break;

  case WORDS_MSG_TIMER:
    if (frxCount.errors) {
      frx_clear();
      break;
    }

    frx_messageReceived = MSG_TYPE_TIMER; // done
    break;

  default:
    break; // continue
  }
}

void IRAM_ATTR frx_sampleInput() {
  bool pin_level = mcu_get_rxPin();
  input_edge = (rx_input == pin_level) ? 0 : (pin_level ? +1 : -1);
  rx_input = pin_level;
}

void IRAM_ATTR frx_tick() {

  if ((IS_P_INPUT && nTicks > veryLongPauseLow_Len)) {
    frx_clear();
  }


  // receive and decode input
  if (!frx_isReceiverBlocked()) {
    frx_tick_receive_message();
  }

  if (pEdge) {
    aTicks = 0;
    pTicks = 0;
    nTicks = 0;
  }

  // measure the time between input edges
  ++aTicks;
  if (IS_P_INPUT) {
    ++pTicks;
  } else {
    ++nTicks;
  }

  if (frx_messageReceived != MSG_TYPE_NONE) {
    frx_MSG_RECEIVED_ISR_cb();
  }
}

#endif

