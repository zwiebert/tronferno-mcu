#include <stdlib.h>

#include "user_config.h"

#include "../userio/inout.h"
#include "user_config.h"
#include "main/common.h"
#include "fernotron/fer.h"
#include "fernotron/fer_code.h"
#include "config/config.h"

struct fer_msg message_buffer;

#ifndef FER_RECEIVER
bool ICACHE_FLASH_ATTR recv_lockBuffer(bool enableLock) {
  return true;
}
#else
static volatile uint8_t requestLock;
static volatile bool isLocked;

bool ICACHE_FLASH_ATTR recv_lockBuffer(bool enableLock) {
  if (enableLock) {
    requestLock++;
    do {
      mcu_delay_us(100);
    } while (!isLocked && recvTick == C.app_recv);
  } else {
    requestLock--;
  }
  return true;
}
#endif

/////////////////////////// interrupt code //////////////////////
#if defined FER_RECEIVER || defined FER_TRANSMITTER
// counters
static uint8_t CountTicks, CountBits;
static uint16_t CountWords;
// buffer to store received RF data
static uint16_t dtRecvBuffer[2];
#define dtSendBuf (dtRecvBuffer[0])

/*  "t if VAL contains an even number of 1 bits" */
static bool IRAM_ATTR is_bits_even(uint8_t val) {
  val ^= val >> 4;
  val ^= val >> 2;
  val ^= val >> 1;
  val &= 0x01;
  return (val == 0);
}

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
static uint8_t IRAM_ATTR fer_get_word_parity(uint8_t data_byte, uint8_t pos) {
  uint8_t result;
  bool is_even = is_bits_even(data_byte);

  result = ((pos & 1)) ? (is_even ? 3 : 1) : (is_even ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
static uint16_t  IRAM_ATTR fer_add_word_parity(uint8_t data_byte, int pos) {
  uint16_t result = (data_byte | (((uint16_t) fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

#endif

#ifdef FER_RECEIVER

#define bitLen               FER_BIT_WIDTH_TCK
// bit is 1, if data edge comes before sample position (/SHORT\..long../)
// bit is 0, if data edge comes after sample position  (/..LONG..\short/)
#define SAMPLE_BIT ((pTicks < FER_BIT_SAMP_POS_TCK))

#define IS_P_INPUT (rx_input == true)
#define IS_N_INPUT (rx_input == false)
#define veryLongPauseLow_Len (2 * FER_STP_WIDTH_MAX_TCK)
#define fuzCmp(a,b) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * 2))
#define fuzzCmp(a,b,n) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * (n)))

#define pEdge (input_edge > 0)
#define nEdge (input_edge < 0)
#define clockEdge pEdge
#define dataEdge nEdge

#define MAX_PRG_TICK_COUNT (DATA_CLOCK_TO_TICKS(FER_PRG_FRAME_WIDTH_DCK) * 2) // FIXME

#define ct_incr(ct, limit) (!((++ct >= limit) ? (ct = 0) : 1))
#define ct_incrementP(ctp, limit) ((++*ctp, *ctp %= limit) == 0)

static int8_t preBits;
static int8_t input_edge; // Usually 0, but -1 or +1 at the tick where input has changed
static uint16_t aTicks, pTicks, nTicks;
#define POS__IN_DATA (CountTicks > 0 && CountBits < bitsPerWord)
#define POS__NOT_IN_DATA ((CountTicks == 0) && (CountBits == 0))
//#define PUTBIT(dst,bit,val) (put_bit_16(&dst, (bit), (val)))
#define PUTBIT(dst,bit,val) ((val) ? (dst |= (1 << (bit))) : (dst &= ~(1 << (bit))))

// holds sampled value of input pin
static bool rx_input, old_rx_input;

static uint32_t prgTickCount;
static int error;

// flags

volatile uint8_t MessageReceived;

#define rbuf (&message_buffer)

#define rxbuf_current_byte() (&rbuf->cmd[0] + received_byte_ct)
static uint16_t received_byte_ct;
static uint16_t bytesToReceive;
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
static bool  IRAM_ATTR fer_word_parity_p(uint16_t word, uint8_t pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}

static fer_error  IRAM_ATTR frx_extract_Byte(const uint16_t *src, uint8_t *dst) {
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

static fer_error  IRAM_ATTR frx_verify_cmd(const uint8_t *dg) {
  int i;
  uint8_t checksum = 0;
  bool all_null = true;

  for (i = 0; i < bytesPerCmdPacket - 1; ++i) {
    checksum += dg[i];
    if (dg[i] != 0)
      all_null = false;
  }

  if (all_null)
    return fer_BAD_CHECKSUM;

  return (checksum == dg[i] ? fer_OK : fer_BAD_CHECKSUM);
}

static void  IRAM_ATTR frx_recv_decodeByte(uint8_t *dst) {
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
    PUTBIT(dtRecvBuffer[CountWords & 1], CountBits, SAMPLE_BIT);
  }

  return preBits == FER_PRE_BIT_CT;
}

static bool  IRAM_ATTR frx_receive_message(void) {

  if (frx_wait_and_sample()) {
    if (ct_incr(CountTicks, bitLen)) {
      if (ct_incr(CountBits, bitsPerWord)) {
        // word complete
        if ((CountWords & 1) == 1) {
          // word pair complete
          frx_recv_decodeByte(rxbuf_current_byte());
          frx_incr_received_bytes();
        }
        ++CountWords;
        if (hasAllBytesReceived()) {
          return true; // success
        }
      }
    }
  }

  return false;  // continue
}

void IRAM_ATTR  frx_clear(void) {
  CountTicks = CountBits = CountWords = 0;
  preBits = 0;
  error = 0;
  MessageReceived = 0;
  received_byte_ct = 0;
  bytesToReceive = BYTES_MSG_PLAIN;

}

static void  IRAM_ATTR frx_tick_receive_message() {
  if (frx_receive_message()) {

    switch (getBytesToReceive()) {

    case BYTES_MSG_PLAIN:
      if ((!error && fer_OK == frx_verify_cmd(rbuf->cmd))) {
        if (FRB_GET_CMD(rbuf->cmd) == fer_cmd_Program && FRB_ADDR_IS_CENTRAL(rbuf->cmd)) {
          bytesToReceive = BYTES_MSG_RTC;
        } else {
          MessageReceived = MSG_TYPE_PLAIN;
        }
      } else {
        frx_clear();
      }
      break;

    case BYTES_MSG_RTC:
      if (FRB_GET_FPR0_IS_RTC_ONLY(rbuf->rtc)) {
        MessageReceived = MSG_TYPE_RTC;

      } else {
        bytesToReceive = BYTES_MSG_TIMER;
      }
      break;

    case BYTES_MSG_TIMER:
      MessageReceived = MSG_TYPE_TIMER;
    }
  }
}

void  IRAM_ATTR frx_tick() {

  // sample input pin and detect input edge
  rx_input = mcu_get_rxPin();
  input_edge = (old_rx_input == rx_input) ? 0 : (IS_P_INPUT ? +1 : -1);
  old_rx_input = rx_input;

  // receive and decode input
  if (!(MessageReceived || is_sendMsgPending || isLocked)) {

    if ((prgTickCount && !--prgTickCount) || (IS_P_INPUT && nTicks > veryLongPauseLow_Len)) {
      frx_clear();
    }

    frx_tick_receive_message();

  }

  if (requestLock) {
    if (!isLocked)
      frx_clear();
    isLocked = true;
  } else {
    isLocked = false;
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
}

#endif

#ifdef FER_TRANSMITTER
/////////////////////////// transmitter /////////////////////////
/////////////////////////// interrupt code //////////////////////
#undef bitLen

#ifdef FER_SENDER_DCK
#define bitLen               FER_BIT_WIDTH_DCK
#define shortPositive_Len    FER_BIT_SHORT_DCK
#define longPositive_Len     FER_BIT_LONG_DCK
#define pre_Len              FER_PRE_WIDTH_DCK
#define pauseHigh_Len        FER_STP_NEDGE_DCK
#else
#define bitLen               FER_BIT_WIDTH_TCK
#define shortPositive_Len    FER_BIT_NEDGE_0_TCK
#define longPositive_Len     FER_BIT_NEDGE_1_TCK
#define pre_Len              FER_PRE_WIDTH_TCK
#define pauseHigh_Len        FER_STP_NEDGE_TCK
#endif

static bool tx_output;   // output line
extern volatile uint16_t wordsToSend;

#define make_Word fer_add_word_parity
#define init_counter() (CountTicks = CountBits = CountWords = 0)
#define ftx_update_output_preamble() (tx_output = (CountTicks < shortPositive_Len))
#define advancePreCounter() (ct_incr(CountTicks, pre_Len) && ct_incr(CountBits, bitsPerPre))
#define advanceStopCounter() (ct_incr(CountTicks, bitLen) && ct_incr(CountBits, bitsPerPause))
#define ftx_update_output_stop() (tx_output = (CountTicks < pauseHigh_Len) && (CountBits == 0))

// sets output line according to current bit in data word
// a stop bit is sent in CountBits 0 .. 2
// a data word is sent in CountBits 3 .. 10
static void  IRAM_ATTR ftx_update_output_data() {
  int bit = CountBits - bitsPerPause;

  if (bit < 0) {  // in stop bit (CountBits 0 .. 2)
    tx_output = CountBits == 0 && CountTicks < shortPositive_Len;
  } else { // in data word
    tx_output = CountTicks < shortPositive_Len || (CountTicks < longPositive_Len && !(dtSendBuf & (1 << bit)));
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
      dtSendBuf = make_Word(txmsg->cmd[0], 0);
    }
  } else {
    // send data words + stop bits
    ftx_update_output_data();
    // counting ticks until end_of_frame
    if (ct_incr(CountTicks, bitLen)) {
      // bit sent
      if ((ct_incr(CountBits, bitsPerWord + bitsPerPause))) {
        // word + stop sent
        if (ct_incr(CountWords, wordsToSend)) {
          // line sent
          stop_done = preamble_done = false;
          return true; // done
        } else {
          dtSendBuf = make_Word(txmsg->cmd[CountWords / 2], CountWords); // load next word
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
    is_sendMsgPending = false;
    init_counter();
    mcu_put_txPin(0);

  }
}

void IRAM_ATTR  ftx_tick(void) {

  if (is_sendMsgPending) {
    ftx_tick_send_message();
  }
}
#endif

