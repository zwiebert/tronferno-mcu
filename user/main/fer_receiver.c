#include <stdlib.h>

#include "common.h"
#include "counter.h"
#include "fer.h"

#include "utils.h"
#include "fer_code.h"

#ifdef FER_RECEIVER

#define DB_FORCE_CMD 0
#define DB_IGNORE_ERRORS 0
#define TEST_CLOCK_EDGE 0
#define TEST_DATA_EDGE 1

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

#define clock_edge (input_edge > 0)
#define data_edge (input_edge < 0)
#define pEdge (input_edge > 0)
#define nEdge (input_edge < 0)

static int8_t input_edge; // Usually 0, but -1 or +1 at tick input change is detected
static uint16_t aTicks, pTicks, nTicks;
#define LATEST_BIT_A_TICKS (aTicks)
#define LATEST_BIT_P_TICKS (pTicks)
#define LATEST_BIT_N_TICKS (nTicks)
static uint16_t nid_pTicks, nid_aTicks;
#define POS__IN_DATA (CountTicks > 0 && CountBits < bitsPerWord)
#define POS__NOT_IN_DATA ((CountTicks == 0) && (CountBits == 0))
#define PUTBIT(dst,bit,val) (put_bit_16(&dst, (bit), (val)))

#if 0
#undef bitLen
#define bitLen ((pre_len * 30 + 5) / 20)
#undef dtSample_Pos
#define dtSample_Pos ((pre_len * 30 + 5) / 40)
#endif

// holds sampled value of input pin
static bool input, old_input;

static uint32_t prgTickCount;
static int error;

// flags
volatile bool has_cmdReceived, has_prgReceived, is_recPrgFrame;

// counters
uint16_t CountRecvErrors;
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

  result = ((pos & 1) != 0) ?
  (is_bits_even (data_byte) ? 3 : 1)
  : (is_bits_even (data_byte) ? 0 : 2);

  return result;
}

/* "extend DATA_BYTE with parity according to POS" */
static uint16_t
fer_add_word_parity (uint8_t data_byte, int pos) {
  uint16_t result= (data_byte | (((uint16_t)fer_get_word_parity(data_byte, pos)) << 8));
  return result;
}

/* "return t if parity is even and postiion matches parity bit \ 1/3
on even positions and 0,2 on odd positions)" */
static bool
fer_word_parity_p (uint16_t word, uint8_t pos) {
  bool result = fer_add_word_parity((word & 0xff), pos) == word;
  return result;
}


static fer_errors
fer_extract_Byte(const uint16_t *src, uint8_t *dst)
{
 #if 0
  if (fer_word_parity_p(src[0], 0)) {
    *dst = src[0];   return fer_OK;
  } else if (fer_word_parity_p(src[1], 1)) {
    *dst = src[1];   return fer_OK;
  }
#elif 0
  *dst = src[0];return fer_OK;
#else
 if (fer_word_parity_p(src[0], 0)
 && fer_word_parity_p(src[1], 1)
 && ((0xff & src[0]) == (0xff & src[1]))) {
      *dst = src[0];   return fer_OK;
 }
#endif
  *dst = 0xff;  // used to be zero, but not good for checksum test
  return fer_BAD_WORD_PARITY;
  
}

fer_errors
fer_verify_cmd(const uint8_t *dg) {
  int i;
  uint8_t checksum = 0;

  for (i=0; i < bytesPerCmdPacket - 1; ++i) {
    checksum += dg[i];
  }
  
  return (checksum == dg[i] ? fer_OK : fer_BAD_CHECKSUM);
}


/* Sample and store current input data bit.  */
static void store_sample() {
    if (data_edge) {
      PUTBIT(dtRecvBuffer[CountWords & 1], CountBits, pTicks < dtSample_Pos);
    }
}

static void recv_decodeByte(uint8_t *dst) {
	if (fer_OK != fer_extract_Byte(dtRecvBuffer, dst)) {
		++error;
		++CountRecvErrors;
	}
}

static bool is_start_bit(unsigned len, unsigned nedge) {
	if ((FER_STP_WIDTH_MIN <= len && len <= FER_STP_WIDTH_MAX) && (FER_STP_NEDGE_MIN <= nedge && nedge <= FER_STP_NEDGE_MAX))
		return true;

	return false;
}

static bool is_pre_bit(unsigned len, unsigned nedge) {
	if ((FER_PRE_WIDTH_MIN <= len && len <= FER_PRE_WIDTH_MAX) && (FER_PRE_NEDGE_MIN <= nedge && nedge <= FER_PRE_NEDGE_MAX)) {
		return true;
    }
	return false;
}

/* returns true if we are inside a start/sync bit. it returns false at
the rising edge of the next data bit. */
// returns: 0
// returns 1 + n:  stop bit behind pre-bits
// returns 1: stop bit behind data-bits
// returns -1: error
// returns -2: ??
#if 1
static int recv_detect_start() {
  // ----____________________________________ (STP_NEDGE:STP_WIDTH) (4:36 @T100)
  // ---_____ || -----___ || ------__ || --______  PRE (7x)
  int result = 0;
  static int preCounter;


  if (pEdge) { 		// one -_/ bit received. analyzing now


    // latest bit was a pre-bit?
    if (is_pre_bit(aTicks, nid_pTicks)) {
      ++preCounter;
      return 0;
    }

    // latest bit was too long
    if (aTicks > (FER_STP_WIDTH_MAX * 2)) {
      result = -1; // error

      // latest bit was the start/stop bit we are looking for
      } else if (is_start_bit(aTicks, nid_pTicks)) {
      result = 1 + preCounter;

      // ???
      } else {
      result = -2;
    }

    preCounter = 0;
    return result;

  }

  return 0;
}


#elif 1
static int recv_detect_start() {
  // ----____________________________________ (STP_NEDGE:STP_WIDTH) (4:36 @T100)
  // ---_____ || -----___ || ------__ || --______  PRE (7x)
  int result = 0;
  static int preCounter;


  if (pEdge) { 		// one -_/ bit received. analyzing now


    // latest bit was a pre-bit?
    if (is_pre_bit(nid_aTicks, nid_pTicks)) {
      ++preCounter;
      return 0;
    }

    // latest bit was too long
    if (nid_aTicks > (FER_STP_WIDTH_MAX * 2)) {
      result = -1; // error

      // latest bit was the start/stop bit we are looking for
      } else if (is_start_bit(nid_aTicks, nid_pTicks)) {
      result = 1 + preCounter;

      // ???
      } else {
      result = -2;
    }

    preCounter = 0;
    return result;

  }

  return 0;
}

#else
static int recv_detect_start() {
  // ----____________________________________ (STP_NEDGE:STP_WIDTH) (4:36 @T100)
  // ---_____ || -----___ || ------__ || --______  PRE (7x)
  int result = 0;
  static int preCounter;


  if (clock_edge) { 		// one -_/ bit received. analyzing now


    // latest bit was a pre-bit?
    if (is_pre_bit(LATEST_BIT_A_TICKS, LATEST_BIT_P_TICKS + 1)) {
      ++preCounter;
      return 0;
    }

    // latest bit was too long
    if (LATEST_BIT_A_TICKS > (FER_STP_WIDTH_MAX * 2)) {
      result = -1; // error

      // latest bit was the start/stop bit we are looking for
      } else if (is_start_bit(LATEST_BIT_A_TICKS, LATEST_BIT_P_TICKS + 1)) {
      result = 1 + preCounter;

      // ???
      } else {
      result = -2;
    }

    preCounter = 0;
    return result;

  }

  return 0;
}


#endif


static bool dg_detect_tooLongPause() { return (IS_P_INPUT && nTicks > veryLongPauseLow_Len); }

static bool cmd_recv(void) {


  #if 1
  if (dg_detect_tooLongPause()) {
    //db_puts("LP");
    CountTicks = CountBits = CountWords = CountLines = 0;
    return false;
  }
  #endif

  // hold counters at zero until pre-bits and stop bits are over
  if (POS__NOT_IN_DATA) {
        if (CountWords == 0 && (recv_detect_start() < 1+5))  return false; //(FIXME: literals)  some pre bits + 1 start bit required
        if (CountWords > 0 && (recv_detect_start() != 1))   return false;  // only start bit required, no pre bits allowed  
    } 
    
      #define POS__IN_DATA (CountTicks > 0 && CountBits < bitsPerWord)

      
      if (POS__IN_DATA) {
         store_sample();  // try sampling data
      }
  
  if (ct_incr(CountTicks, bitLen)) {
    if (ct_incr(CountBits, bitsPerWord)) {
      if ((CountWords & 1)) {
        recv_decodeByte(&dtRecvCmd[(CountWords - 1) / 2]);
      }
      if (ct_incr(CountWords, wordsPerCmdPacket)) {
        return true;
      }
    }
  }
 
  return false;
}

#define MAX_PRG_TICK_COUNT ((FER_PRG_FRAME_WIDTH_T100 * FER_TICK_FREQ_MULT)  + (100 * FER_TICK_FREQ_MULT))

/* Called by tick interrupt handler. Stores state and data internally
 between calls.  We are called again and again at every tick until
 receiving is complete. The internal data counter and the tick timer
 interrupt are both automatically configured according to
 FER_FREQ_MULT

 Returns 0 while sampling and storing data is in progress. The tick
 handler is supposed to call again every tick until non zero is
 returned.

 Return 1 when done successfully (counter overflow).

 May return -1 at any time if an error has occurred (e.g. a long pause is detected instead of a short start/sync-bit.

 When returning -1 the internal state is reset, except for ERROR. */

static int prg_recv(void) {
	int result = 0;
	// counters should be zero at fist call on new data
	precond(prgTickCount != 0 || !(CountTicks || CountBits || CountWords || CountLines));

	// reset error
	if (prgTickCount++ == 0) {
		error = 0;
	}

#if 0 //FIXME
	// timeout (e.g. signal loss)
	if (prgTickCount > MAX_PRG_TICK_COUNT) {
		goto err;
	}
#endif

	// there is no pause inside a prg sequence
	if (dg_detect_tooLongPause()) {
      #if ! DB_IGNORE_ERRORS
		goto err;
        #endif
	}

	// hold data-word counter at current begin of word positions until
	// start/sync bit is over
	if (POS__NOT_IN_DATA) {
		if (recv_detect_start() < 1)
			goto cont;
		// db_puts("S");
	} 
	  store_sample();  // try to store sample
  
  
  // counting ticks until end_of_frame
    if (ct_incr(CountTicks, bitLen)) {
      if (ct_incr(CountBits, bitsPerWord)) {
        if ((CountWords & 1)) {
          recv_decodeByte(&dtRecvPrgFrame[CountLines][(CountWords - 1) / 2]);
        }
        if (ct_incr(CountWords, wordsPerPrgLine)) {
              if ((CountLines == 0 &&  FRB_GET_FPR0_IS_RTC_ONLY(dtRecvPrgFrame[FPR_RTC_START_ROW])) 
              || ct_incr(CountLines, linesPerPrg)) {
                #if ! DB_IGNORE_ERRORS
                  if (error || CountRecvErrors)
                  goto err;
                  #endif
                  result = 1;
                  goto succ;
          }
        }
      }
    }
    
	goto cont;
	err:
   
 #if !DB_IGNORE_ERRORS
     error = 1;
	result = -1;
 #else
 goto cont;
 #endif
	succ:
	//io_puts(error ? "Pbd\n" :"Pgd\n");
	prgTickCount = 0;
	CountTicks = CountBits = CountWords = CountLines = 0;
	cont: return result;
}

// receive the command and set flags if rtc or timer data will follow
static void tick_recv_command() {
	if (!has_cmdReceived && !is_recPrgFrame && !has_prgReceived) {
		bool done = cmd_recv();

		if (done) {
          #if DB_FORCE_CMD
          has_cmdReceived = true;
          #else
			if (fer_OK == fer_verify_cmd(dtRecvCmd)) {
				if (FRB_GET_CMD(dtRecvCmd) == fer_cmd_Program && FRB_MODEL_IS_CENTRAL(dtRecvCmd)) {
					// timer programming block will follow
                    init_counter();
					is_recPrgFrame = true;
				} else {
					has_cmdReceived = true;
				}
        
			}
         #endif
		}
	}
}

#ifndef FER_RECEIVER_MINIMAL
static void tick_recv_programmingFrame() {
	int result;

	if (is_recPrgFrame && !has_prgReceived && (result = prg_recv())) {

		if (result > 0)
			has_prgReceived = true;

		is_recPrgFrame = false;
	}
}
#endif


void tick_ferReceiver() {
  if (is_sendCmdPending || is_sendPrgPending) // FIXME: to avoid receiving our own transmission (good coding?)
    return;
  
  input = fer_get_recvPin();
  
  
  input_edge = (old_input == input) ? 0 : (IS_P_INPUT ? +1 : -1);
  old_input = input;
  

   
  if (POS__NOT_IN_DATA) {

	 ++nid_aTicks;
	  if (IS_P_INPUT) {
     ++nid_pTicks;
  }

  }
  
  
  tick_recv_command();
  #ifndef FER_RECEIVER_MINIMAL
  tick_recv_programmingFrame();
  #endif

  ++aTicks;
  if (IS_P_INPUT) {
    ++pTicks;
    } else {
    ++nTicks;
  }
  
  if (clock_edge) {
    nid_pTicks = 0;
	nid_aTicks = 0;
    aTicks = 0;
    pTicks = 0;
    } else if (data_edge) {
    nTicks = 0;
  }
}

#endif
