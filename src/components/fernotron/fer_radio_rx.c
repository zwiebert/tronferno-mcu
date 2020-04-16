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
#else

#endif


#ifdef FTRX_COMMON_COUNTERS
#endif

struct ftrx_counter {
  u16 Words;
  u8 Ticks, Bits;
};

/////////////////////////// interrupt code //////////////////////
#if defined FER_RECEIVER || defined FER_TRANSMITTER
// buffer to store received RF data
static u16 dtRecvBuffer[2];

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

#endif

#ifdef FER_RECEIVER

static struct ftrx_counter frxCount;

#define bitLen               DATA_CLOCK_TO_TICKS(FER_BIT_WIDTH_DCK)
// bit is 1, if data edge comes before sample position (/SHORT\..long../)
// bit is 0, if data edge comes after sample position  (/..LONG..\short/)
#define SAMPLE_BIT ((pTicks < DATA_CLOCK_TO_TICKS(FER_BIT_SAMP_POS_DCK)))

#define IS_P_INPUT (rx_input == true)
#define IS_N_INPUT (rx_input == false)
#define veryLongPauseLow_Len (2 * DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MAX_DCK))

#define pEdge (input_edge > 0)
#define nEdge (input_edge < 0)
#define clockEdge pEdge
#define dataEdge nEdge

#define MAX_PRG_TICK_COUNT (DATA_CLOCK_TO_TICKS(FER_PRG_FRAME_WIDTH_DCK) * 2) // FIXME

#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)

static i8 preBits;
static i8 input_edge; // Usually 0, but -1 or +1 at the tick where input has changed
static u16 aTicks, pTicks, nTicks;
#define POS__IN_DATA (frxCount.Ticks > 0 && frxCount.Bits < FER_CMD_BIT_CT)
#define POS__NOT_IN_DATA ((frxCount.Ticks == 0) && (frxCount.Bits == 0))
//#define PUTBIT(dst,bit,val) (put_bit_16(&dst, (bit), (val)))
#define PUTBIT(dst,bit,val) ((val) ? (dst |= (1 << (bit))) : (dst &= ~(1 << (bit))))

// holds sampled value of input pin
static bool rx_input, old_rx_input;

static u32 prgTickCount;
static int error;

// flags

volatile u8 frx_messageReceived;

#define rxbuf_current_byte() (&rxmsg->cmd.bd[0] + received_byte_ct)
static u16 received_byte_ct;
static u16 bytesToReceive;
#define getBytesToReceive() (bytesToReceive + 0)
#define hasAllBytesReceived() (bytesToReceive <= received_byte_ct)

static void  IRAM_ATTR frx_incr_received_bytes(void) {
  if (received_byte_ct <= bytesToReceive) { // FIXME:
    ++received_byte_ct;
  } else {

  }
}

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
    ++error;
  }
}

static bool  IRAM_ATTR frx_is_stopBit(unsigned len, unsigned nedge) {
  return ((FER_STP_WIDTH_MIN_TCK <= len && len <= FER_STP_WIDTH_MAX_TCK) && (FER_STP_NEDGE_MIN_TCK <= nedge && nedge <= FER_STP_NEDGE_MAX_TCK));
}

static bool  IRAM_ATTR frx_is_pre_bit(unsigned len, unsigned nedge) {
  return ((FER_PRE_WIDTH_MIN_TCK <= len && len <= FER_PRE_WIDTH_MAX_TCK) && (FER_PRE_NEDGE_MIN_TCK <= nedge && nedge <= FER_PRE_NEDGE_MAX_TCK));
}

static bool  IRAM_ATTR frx_wait_and_sample(void) {

  if (POS__NOT_IN_DATA) {

    if (pEdge) {

      if (preBits < FER_PRE_BIT_CT) {

        // wait until preamble is over
        if (frx_is_pre_bit(aTicks, pTicks) && ++preBits) {
          return false;
        }

      } else {

        // wait until stopBit is over
        if (!frx_is_stopBit(aTicks, pTicks))
          return false; // continue
      }

    } else {
      return false;
    }

  } else if (dataEdge) {
    PUTBIT(dtRecvBuffer[frxCount.Words & 1], frxCount.Bits, SAMPLE_BIT);
  }

  return preBits == FER_PRE_BIT_CT;
}

static bool  IRAM_ATTR frx_receive_message(void) {

  if (frx_wait_and_sample()) {
    if (ct_incr(frxCount.Ticks, bitLen)) {
      if (ct_incr(frxCount.Bits, FER_CMD_BIT_CT)) {
        // word complete
        if ((frxCount.Words & 1) == 1) {
          // word pair complete
          frx_recv_decodeByte(rxbuf_current_byte());
          frx_incr_received_bytes();
        }
        ++frxCount.Words;
        if (hasAllBytesReceived()) {
          return true; // success
        }
      }
    }
  }

  return false;  // continue
}

void IRAM_ATTR  frx_clear(void) {
  frxCount = (struct ftrx_counter) {};
  preBits = 0;
  error = 0;
  frx_messageReceived = 0;
  received_byte_ct = 0;
  bytesToReceive = BYTES_MSG_PLAIN;

}

static void IRAM_ATTR frx_tick_receive_message() {
  if (frx_receive_message()) {

    switch (getBytesToReceive()) {

    case BYTES_MSG_PLAIN:
      if ((!error && fer_OK == frx_verify_cmd(rxmsg->cmd.bd))) {
        if (FRB_GET_CMD(rxmsg->cmd.bd) == fer_cmd_Program && FRB_ADDR_IS_CENTRAL(rxmsg->cmd.bd)) {
          bytesToReceive = BYTES_MSG_RTC;
        } else {
          frx_messageReceived = MSG_TYPE_PLAIN;
        }
      } else {
        frx_clear();
      }
      break;

    case BYTES_MSG_RTC:
      if (FRB_GET_FPR0_IS_RTC_ONLY(rxmsg->rtc.bd)) {
        frx_messageReceived = MSG_TYPE_RTC;

      } else {
        bytesToReceive = BYTES_MSG_TIMER;
      }
      break;

    case BYTES_MSG_TIMER:
      frx_messageReceived = MSG_TYPE_TIMER;
    }
  }
}

void IRAM_ATTR frx_sampleInput() {
  rx_input = mcu_get_rxPin();
}
void  IRAM_ATTR frx_tick() {
  // sample input pin and detect input edge

  input_edge = (old_rx_input == rx_input) ? 0 : (IS_P_INPUT ? +1 : -1);
  old_rx_input = rx_input;

  // receive and decode input
  if (!frx_isReceiverBlocked()) {

    if ((prgTickCount && !--prgTickCount) || (IS_P_INPUT && nTicks > veryLongPauseLow_Len)) {
      frx_clear();
    }

    frx_tick_receive_message();

  }

  // measure the time between input edges
  ++aTicks;
  if (IS_P_INPUT) {
    ++pTicks;
  } else {
    ++nTicks;
  }

  if (clockEdge) {
    aTicks = 0;
    pTicks = 0;
  } else if (dataEdge) {
    nTicks = 0;
  }

  if (frx_messageReceived != MSG_TYPE_NONE) {
    frx_MSG_RECEIVED_ISR_cb();
  }
}

#endif

