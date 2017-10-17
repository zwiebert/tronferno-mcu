#include <stdlib.h>

#include "common.h"
#include "counter.h"
#include "fer.h"

#include "utils.h"
#include "fer_code.h"

#ifdef FER_RECEIVER

#define DB_FORCE_CMD 0
#define DB_IGNORE_RTC_ONLY 0
#define DB_IGNORE_ERRORS 0


#define IS_EO_BIT (CountTicks == 0)
#define IS_EO_WORD (IS_EO_BIT && CountBits == 0)
#define IS_EO_LINE (IS_EO_WORD && CountWords == 0)
#define IS_EO_FRAME (IS_EO_LINE && CountLines == 0)


/////////////////////////// interrupt code //////////////////////
#define IS_P_INPUT (input == true)
#define IS_N_INPUT (input == false)
#define veryLongPauseLow_Len (2 * FER_STP_WIDTH_MAX)
#define  fuzCmp(a,b) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * 2))
#define  fuzzCmp(a,b,n) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * (n)))

#define pEdge (input_edge > 0)
#define nEdge (input_edge < 0)
#define clockEdge pEdge
#define dataEdge nEdge


#define MAX_PRG_TICK_COUNT (REL_TO_TICKS(FER_PRG_FRAME_WIDTH_REL) * 100) // FIXME


static int8_t input_edge; // Usually 0, but -1 or +1 at tick input change is detected
static uint16_t aTicks, pTicks, nTicks;
#define POS__IN_DATA (CountTicks > 0 && CountBits < bitsPerWord)
#define POS__NOT_IN_DATA ((CountTicks == 0) && (CountBits == 0))
//#define PUTBIT(dst,bit,val) (put_bit_16(&dst, (bit), (val)))
#define PUTBIT(dst,bit,val) ((val) ? (dst |= (1 << (bit))) : (dst &= ~(1 << (bit))))

// holds sampled value of input pin
static bool input, old_input;

static uint32_t prgTickCount;
static int error;

// flags
volatile bool has_cmdReceived, has_prgReceived;
static bool is_recPrgFrame;

// counters
static uint8_t CountTicks, CountBits, CountWords, CountLines;

// buffer to store received RF data
static uint16_t dtRecvBuffer[2];
static uint8_t dtRecvCmd[bytesPerCmdPacket];
uint8_t dtRecvPrgFrame[linesPerPrg][bytesPerPrgLine];

ferCmdBuf_type ICACHE_FLASH_ATTR get_recvCmdBuf(void) {
	return dtRecvCmd;
}

uint8_t * ICACHE_FLASH_ATTR get_recvPrgBufLine(uint8_t idx) {
	return dtRecvPrgFrame[idx];
}

static void init_counter() {
  CountTicks = CountBits = CountWords = CountLines = 0;
}

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
static uint8_t
fer_get_word_parity (uint8_t data_byte, uint8_t pos) {
  uint8_t result;
  bool is_even = is_bits_even (data_byte);

  result = ((pos & 1)) ?  (is_even ? 3 : 1)  : (is_even ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
static uint16_t
fer_add_word_parity (uint8_t data_byte, int pos) {
  uint16_t result= (data_byte | (((uint16_t)fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

/* "return t if parity is even and position matches parity bit \ 1/3
on even positions and 0,2 on odd positions)" */
static bool
fer_word_parity_p (uint16_t word, uint8_t pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}

static fer_errors fer_extract_Byte(const uint16_t *src, uint8_t *dst) {
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

static fer_errors
fer_verify_cmd(const uint8_t *dg) {
  int i;
  uint8_t checksum = 0;

  for (i=0; i < bytesPerCmdPacket - 1; ++i) {
    checksum += dg[i];
  }

  return (checksum == dg[i] ? fer_OK : fer_BAD_CHECKSUM);
}


static void recv_decodeByte(uint8_t *dst) {
	if (fer_OK != fer_extract_Byte(dtRecvBuffer, dst)) {
		++error;
	}
}

static bool is_stopBit(unsigned len, unsigned nedge) {
	return ((FER_STP_WIDTH_MIN <= len && len <= FER_STP_WIDTH_MAX) && (FER_STP_NEDGE_MIN <= nedge && nedge <= FER_STP_NEDGE_MAX));
}

static bool is_pre_bit(unsigned len, unsigned nedge) {
  return ((FER_PRE_WIDTH_MIN <= len && len <= FER_PRE_WIDTH_MAX) && (FER_PRE_NEDGE_MIN <= nedge && nedge <= FER_PRE_NEDGE_MAX));
}

static int8_t preBits;

static bool wait_and_sample(void) {

	if (POS__NOT_IN_DATA) {
		// hold counters at zero until preamble and/or stop-bit is over
		if (!pEdge) {
			return false;

		} else if (preBits < FER_PRE_BIT_COUNT) {
			if (is_pre_bit(aTicks, pTicks) && ++preBits)
				return 0;

		} else {
			// wait until stopBit is over
			if (!is_stopBit(aTicks, pTicks))
				return 0; // continue
		}

	} else if (dataEdge) {
		PUTBIT(dtRecvBuffer[CountWords & 1], CountBits, pTicks < dtSample_Pos);
	}


	return preBits == FER_PRE_BIT_COUNT;
}

static bool cmd_recv(void) {

  if (!wait_and_sample())
	   return false;
  
  if (ct_incr(CountTicks, bitLen)) {
    if (ct_incr(CountBits, bitsPerWord)) {
    	// word complete
      if ((CountWords & 1)) {
        recv_decodeByte(&dtRecvCmd[(CountWords - 1) / 2]);
      }
      if (ct_incr(CountWords, wordsPerCmdPacket)) {
        return true; // success
      }
    }
  }
  
  return false;  // continue
}

static bool prg_recv(void) {

  if (!wait_and_sample())
	   return false;

  if (ct_incr(CountTicks, bitLen)) {
    if (ct_incr(CountBits, bitsPerWord)) {
    	// word complete
      if ((CountWords & 1)) { 
         recv_decodeByte(&dtRecvPrgFrame[CountLines][(CountWords - 1) / 2]); 
         }
      if (ct_incr(CountWords, wordsPerPrgLine)) {
        // line complete
    	  if (!DB_IGNORE_RTC_ONLY && CountLines == 0 &&  FRB_GET_FPR0_IS_RTC_ONLY(dtRecvPrgFrame[FPR_RTC_START_ROW]))
    		  return true;
        if (ct_incr(CountLines, linesPerPrg)) {
        	return true;

          
        }
      }
    }
  }
  return false; // continue
}

void fer_recvClearAll(void) {
	init_counter();
	preBits = 0;
	error = 0;
	has_cmdReceived = false;
	has_prgReceived = false;
	is_recPrgFrame = false;

}

// receive the command and set flags if rtc or timer data will follow
static void tick_recv_command() {
	if (!has_cmdReceived && !is_recPrgFrame && !has_prgReceived) {
		bool done = cmd_recv();

		if (done) {
          #if DB_FORCE_CMD
          has_cmdReceived = true;
          #else
			if (DB_IGNORE_ERRORS || (!error && fer_OK == fer_verify_cmd(dtRecvCmd))) {
				if (FRB_GET_CMD(dtRecvCmd) == fer_cmd_Program && FRB_MODEL_IS_CENTRAL(dtRecvCmd)) {
					// timer programming block will follow
					is_recPrgFrame = true;
                    prgTickCount = MAX_PRG_TICK_COUNT;
				} else {
					has_cmdReceived = true;
				}      
			} else {
				fer_recvClearAll();
            }                          
         #endif
		}
	}
}

#ifndef FER_RECEIVER_MINIMAL
static void tick_recv_programmingFrame() {
  if (is_recPrgFrame && !has_prgReceived) {
	  bool done = prg_recv();

	  if (done) {
	      has_prgReceived = true;
	  }
  }
}
#endif

void tick_ferReceiver() {
  if (is_sendCmdPending || is_sendPrgPending) // FIXME: to avoid receiving our own transmission (good coding?)
  return;
  
  input = fer_get_recvPin();
  
  
  input_edge = (old_input == input) ? 0 : (IS_P_INPUT ? +1 : -1);
  old_input = input;

  if ((prgTickCount && !--prgTickCount) || (IS_P_INPUT && nTicks > veryLongPauseLow_Len)) {
	  fer_recvClearAll();
  }




  
  #ifndef FER_RECEIVER_MINIMAL
  tick_recv_programmingFrame();
  #endif

  tick_recv_command();


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
