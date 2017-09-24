#include <stdlib.h>

#include "common.h"
#include "counter.h"
#include "fer.h"

#include "codec.h"
#include "utils.h"
#include "inout.h"
#include "fer_code.h"

#ifdef FER_RECEIVER

#define DB_FORCE_CMD 0
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

uint32_t data_clock_ticks = DEFAULT_DATA_CLOCK_TICKS;
uint32_t pre_len, pre_nedge;
#define clock_edge (input_edge > 0)
#define data_edge (input_edge < 0)
static int8_t input_edge; // Usually 0, but -1 or +1 at tick input change is detected
static bool old_input;
static uint16_t pos_width_ticks, neg_width_ticks;
//static volatile uint16_t old_pos_width_ticks, old_neg_width_ticks;
#define LATEST_BIT_A_TICKS (LATEST_BIT_P_TICKS + LATEST_BIT_N_TICKS)
#define LATEST_BIT_P_TICKS (pos_width_ticks)
#define LATEST_BIT_N_TICKS (neg_width_ticks)
static uint16_t test_highCount;
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
static bool input;

static uint32_t prgTickCount;
static int error;

// flags
volatile bool has_cmdReceived, has_prgReceived;
volatile bool is_recPrgFrame;
volatile bool has_prgReceived;

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


/* Sample and store current input data bit.  */
static void store_sample() {
    if (data_edge) {
      PUTBIT(dtRecvBuffer[CountWords % 2], CountBits, pos_width_ticks < dtSample_Pos);
      data_clock_ticks = CountBits;
    }
}

static void recv_decodeByte(uint8_t *dst) {
	if (fer_OK != fer_extract_Byte(dtRecvBuffer, dst)) {
		++error;
		++CountRecvErrors;
	}
}


static void advance_Counter(uint8_t wordsPerLine) {

 #if TEST_CLOCK_EDGE   
        if (clock_edge) {
          if (CountTicks > bitLen / 2) {
             CountTicks = bitLen;
          }          
        }
  #endif      

	(void)((ct_incr(CountTicks, bitLen)) 
    && (ct_incr(CountBits, bitsPerWord))
     && (ct_incr(CountWords, wordsPerLine))
     && (ct_incr(CountLines, linesPerPrg)));
}


static bool is_start_bit(unsigned len, unsigned nedge) {
	if ((FER_STP_WIDTH_MIN <= len && len <= FER_STP_WIDTH_MAX) && (FER_STP_NEDGE_MIN <= nedge && nedge <= FER_STP_NEDGE_MAX))
		return true;

	return false;
}

static bool is_pre_bit(unsigned len, unsigned nedge) {
	if ((FER_PRE_WIDTH_MIN <= len && len <= FER_PRE_WIDTH_MAX) && (FER_PRE_NEDGE_MIN <= nedge && nedge <= FER_PRE_NEDGE_MAX)) {
      pre_len = len; pre_nedge = nedge;
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
static int recv_detect_start() {
  // ----____________________________________ (STP_NEDGE:STP_WIDTH) (4:36 @T100)
  // ---_____ || -----___ || ------__ || --______  PRE (7x)
  int result = 0;
  static int preCounter;
  
  
  if (clock_edge) { 		// one -_/ bit received. analyzing now


    // latest bit was a pre-bit?
    if (is_pre_bit(LATEST_BIT_A_TICKS, test_highCount + 1)) {
      ++preCounter;
      return 0;
    }
    
    // latest bit was too long
    if (LATEST_BIT_A_TICKS > (FER_STP_WIDTH_MAX * 2)) { 
      result = -1; // error
      
      // latest bit was the start/stop bit we are looking for
      } else if (is_start_bit(LATEST_BIT_A_TICKS, test_highCount + 1)) {
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



static bool dg_detect_tooLongPause() { return (IS_P_INPUT && neg_width_ticks > veryLongPauseLow_Len); }

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
    if (CountWords == 0) {
      if (recv_detect_start() < 1 + 5)
        return false; // less than 7 pre-bits received
    } else {
      if (recv_detect_start() != 1) 
         return false;  // stop bit is not over
    }    
    }  else { // POS__IN_DATA
      store_sample();  // try sampling data
  }


  advance_Counter(wordsPerCmdPacket);
  
  if (IS_EO_LINE) {
   CountLines = 0;
   return true;
   
    } else if (IS_EO_WORD) {
    if ((CountWords % 2) != 0) {
      recv_decodeByte(&dtRecvCmd[CountWords / 2]);
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
		goto err;
	}

	// hold data-word counter at current begin of word positions until
	// start/sync bit is over
	if (POS__NOT_IN_DATA) {
		if (recv_detect_start() < 1)
			goto cont;
		// db_puts("S");
	} else {
	  store_sample();  // try to store sample
    }
  
  // counting ticks until end_of_frame
  advance_Counter(wordsPerPrgLine);
  
  if (IS_EO_WORD) {
    bool rtc_frame = false;

    if (IS_EO_LINE && CountLines == 1 &&  FRB_GET_FPR0_IS_RTC_ONLY(dtRecvPrgFrame[FPR_RTC_START_ROW])) {
      CountLines = 0;
     rtc_frame = true;
    }
    
    if (IS_EO_FRAME || rtc_frame) {
      if (error || CountRecvErrors)
      goto err;
      result = 1;
      goto succ;
      
      } else if ((CountWords % 2) != 0) {
      // store received words into byte array
         recv_decodeByte(&dtRecvPrgFrame[CountLines][CountWords / 2]);
      }  
  }


	goto cont;
	err: error = 1;
	result = -1;
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
  
  
  if (IS_P_INPUT) {
    ++pos_width_ticks;
    } else {
    ++neg_width_ticks;
  }
   
  if (POS__NOT_IN_DATA && IS_P_INPUT) {
    ++test_highCount;
  }
  
  
  tick_recv_command();
  #ifndef FER_RECEIVER_MINIMAL
  tick_recv_programmingFrame();
  #endif

  if (clock_edge) {
    test_highCount = 0;
    pos_width_ticks = 0;
    } else if (data_edge) {
    neg_width_ticks = 0;
  }
}

#endif
