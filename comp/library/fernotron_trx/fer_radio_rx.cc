#include "app_config/proj_app_cfg.h"
#include <fernotron_trx/raw/fer_radio_trx.h>
#include "fer_trx_impl.hh"
#include "fer_radio_parity.h"
#include <fernotron_trx/raw/fer_msg_plain.h>
#include "fernotron_trx/isr_timer_config.h"
#include <stdlib.h>
#include "fer_app_cfg.h"
#include "fernotron_trx/raw/fer_rawmsg_buffer.h"
#include "fernotron_trx/raw/fer_msg_tx.h"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "fernotron_trx/raw/fer_radio_timings_us.h"

#define US2TCK(us) FER_RX_US_TO_TCK(us)

/// \brief  Possible errors/warnings when receiving a message
/// \note   The only hard error is BAD_CHECKSUM
enum fer_error {
  fer_OK, ///< All is good
  fer_PAIR_NOT_EQUAL, ///< Warning: Each byte is sent twice, as a pair. This error occurs if they are not equal.
  fer_BAD_WORD_PARITY, ///< Warning: A Byte parity bit wrong
  fer_BAD_CHECKSUM ///< Error: One or more of the embedded checksums are wrong
} ;

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
  u16 Words, stopBits;
  u8 Bits, preBits;
  u8 errors; u8 recovered;
};
static bool input_level, input_edge_pos, input_edge_neg;
static u16 aTicks, pTicks, nTicks;
static u16 wordsToReceive;
// flags
volatile fer_msg_type fer_rx_messageReceived;
static bool fer_rx_messageReceived_flag;
static u16 word_pair_buffer[2];
static struct fer_rx_counter frxCount;


void fer_rx_getQuality(struct fer_rx_quality *dst) {
  dst->bad_pair_count = frxCount.recovered;
}

/////////////////////////// interrupt code //////////////////////
#define bitLen               US2TCK(FER_BIT_WIDTH_US)
// bit is 1, if data edge comes before sample position (/SHORT\..long../)
// bit is 0, if data edge comes after sample position  (/..LONG..\short/)
#define SAMPLE_BIT ((pTicks < US2TCK(FER_BIT_SAMP_POS_US)))
#define veryLongPauseLow_Len (2 * US2TCK(FER_STP_WIDTH_MAX_US))
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
  return ((US2TCK(FER_STP_WIDTH_MIN_US) <= len && len <= US2TCK(FER_STP_WIDTH_MAX_US)) && (US2TCK(FER_STP_NEDGE_MIN_US) <= nedge && nedge <= US2TCK(FER_STP_NEDGE_MAX_US)));
}

static bool  IRAM_ATTR fer_rx_is_pre_bit(unsigned len, unsigned nedge) {
  return ((US2TCK(FER_PRE_WIDTH_MIN_US) <= len && len <= US2TCK(FER_PRE_WIDTH_MAX_US)) && (US2TCK(FER_PRE_NEDGE_MIN_US) <= nedge && nedge <= US2TCK(FER_PRE_NEDGE_MAX_US)));
}

static bool  IRAM_ATTR fer_rx_is_dataBitOne(unsigned len, unsigned nedge) {
  return len > (2 * nedge);
}


static bool  IRAM_ATTR fer_rx_is_dataBitZero(unsigned len, unsigned nedge) {
  return len < (2 * nedge);
}

static bool  IRAM_ATTR fer_rx_is_dataBit(unsigned len, unsigned nedge) {
  return FER_BIT_WIDTH_MIN_US <= len && len <= FER_BIT_WIDTH_MAX_US;
}


#define E1 1 // if 1 use experimental receiver code
#define E2 0 // if 1 use tx-output-pin to signal frxCount.stopBits > 0


#if E1 == 1
static bool IRAM_ATTR fer_rx_wait_and_sample(void) {
  if (!input_edge_pos)
    return false;

  const bool isStopBit = fer_rx_is_stopBit(aTicks, pTicks);
  const bool isDataBit = fer_rx_is_dataBit(aTicks, pTicks);
  const bool isDataBitOne = fer_rx_is_dataBitOne(aTicks, pTicks);
  const bool isDataBitZero = fer_rx_is_dataBitZero(aTicks, pTicks);

  if (isStopBit) {
    if (frxCount.stopBits == 0) {
      fer_rx_clear();
    }
    frxCount.Words = frxCount.stopBits;
    ++frxCount.stopBits;
    return false;
  }

  if (frxCount.stopBits == 0)
    return false;

  if (!isDataBit && !(isDataBitOne || isDataBitZero)) {
    fer_rx_clear();
    return false;
  }

  PUT_BIT(word_pair_buffer[frxCount.Words & 1], frxCount.Bits, isDataBitOne);
  return true;
}
#else
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
#endif

static int IRAM_ATTR fer_rx_receive_message(void) {

  if (fer_rx_wait_and_sample()) {
    if (ct_incr(frxCount.Bits, FER_CMD_BIT_CT)) {
      // word complete
      if ((frxCount.Words & 1) == 1) {
        // word pair complete
        fer_rx_recv_decodeByte(rxbuf_current_byte());
      }
      frxCount.Words = frxCount.stopBits;

      return frxCount.Words;
    }
  }
#if E2 == 1
  void ftrx_testSetOutputLevel(bool level);
  ftrx_testSetOutputLevel(frxCount.stopBits > 0);
#endif
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

    if (fer_rx_msg->cmd.sd.cmd.cmd == fer_cmd_Program && FER_CMD_ADDR_IS_CENTRAL(&fer_rx_msg->cmd.sd.cmd)) {
      wordsToReceive = WORDS_MSG_RTC; // continue
    } else {
      fer_rx_messageReceived = MSG_TYPE_PLAIN; // done
      fer_rx_messageReceived_flag = true;
    }
    break;

  case WORDS_MSG_RTC:
    if (frxCount.errors) {
      fer_rx_clear();
      break;
    }

    if (fer_rx_msg->rtc.sd.wd2.sd.rtc_only) {
      fer_rx_messageReceived = MSG_TYPE_RTC;  // done
      fer_rx_messageReceived_flag = true;
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
    fer_rx_messageReceived_flag = true;
    break;

  default:
    break; // continue
  }
}

static void IRAM_ATTR fer_rx_sampleInput(bool pin_level) {

  input_edge_pos = input_edge_neg = false;
  if (input_level != pin_level) {
    if (pin_level)
      input_edge_pos = true;
    else
      input_edge_neg = true;
  }

  input_level = pin_level;
}

void IRAM_ATTR fer_rx_tick(bool pin_level) {

  fer_rx_sampleInput(pin_level);

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

  if (fer_rx_messageReceived_flag) {
    fer_rx_messageReceived_flag = false;
    fer_rx_msg_received_isr_cb();
  }
}

#endif

