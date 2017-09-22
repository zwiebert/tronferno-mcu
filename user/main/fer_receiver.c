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
#define TEST_DATA_EDGE 0

/////////////////////////// interrupt code //////////////////////

#define veryLongPauseLow_Len (2 * FER_STP_WIDTH_MAX)
#define  fuzCmp(a,b) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * 2))
#define  fuzzCmp(a,b,n) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * (n)))

uint32_t data_clock_ticks = DEFAULT_DATA_CLOCK_TICKS;
uint32_t pre_len, pre_phase;
static bool clock_edge, data_edge;
static bool old_input;
static int16_t data_edge_ticks;

#if 0
#undef bitLen
#define bitLen ((pre_len * 30 + 5) / 20)
#undef dtSample_Pos
#define dtSample_Pos ((pre_len * 30 + 5) / 40)
#endif

// holds sampled value of input pin
static bool input;

static uint32_t prgCallCount;
static int error;

// flags
volatile bool has_cmdReceived, has_prgReceived;
volatile bool is_recPrgFrame;
volatile bool has_prgReceived;

// counters
uint16_t CountRecvErrors;
static uint8_t CountTicks, CountBits, CountWords, CountLines;

// received data goes here
static uint16_t dtRecvBuffer[2];
static uint8_t dtRecvCmd[bytesPerCmdPacket];
uint8_t dtRecvPrgFrame[linesPerPrg][bytesPerPrgLine];

ferCmdBuf_type ICACHE_FLASH_ATTR get_recvCmdBuf(void) {
	return dtRecvCmd;
}
uint8_t * ICACHE_FLASH_ATTR get_recvPrgBufLine(uint8_t idx) {
	return dtRecvPrgFrame[idx];
}

#define PUTBIT(dst,bit,val) (put_bit_16(&dst, (bit), (val)))
#define POS__NOT_IN_DATA ((CountTicks == 0) && (CountBits == 0))

/* Sample and store current input data bit.  */
static void store_sample() {
  #if TEST_DATA_EDGE
    if (data_edge) {
      PUTBIT(dtRecvBuffer[CountWords % 2], CountBits, data_edge_ticks < dtSample_Pos);
      data_edge = 0;
      data_clock_ticks = CountBits;
    }
  #else
	if ((CountTicks == dtSample_Pos)) {
		PUTBIT(dtRecvBuffer[CountWords % 2], CountBits, !input);
	}
#endif
}

static void recv_decodeByte(uint8_t *dst) {
	if (fer_OK != fer_extract_Byte(dtRecvBuffer, dst)) {
		++error;
		++CountRecvErrors;
	}
}

static bool advance_CmdCounter() {
	bool end_of_bit = false, end_of_word = false, end_of_line = false;
 #if TEST_CLOCK_EDGE   
        if (clock_edge) {
          if (CountTicks > bitLen / 2) {
             CountTicks = bitLen;
          }          
          clock_edge = false;
        }
  #endif      

	(end_of_bit = ct_incr(CountTicks, bitLen)) && (end_of_word = ct_incr(CountBits, bitsPerWord)) && (end_of_line = ct_incr(CountWords, wordsPerCmdPacket));

	if (!end_of_line) {

		if (end_of_word && (CountWords % 2) != 0) {
			recv_decodeByte(&dtRecvCmd[CountWords / 2]);
		}

	}
	return end_of_line;
}

static bool is_start_bit(int len, int phase) {
	if ((FER_STP_WIDTH_MIN <= len && len <= FER_STP_WIDTH_MAX) && (FER_STP_NEDGE_MIN <= phase && phase <= FER_STP_NEDGE_MAX))
		return true;

	return false;
}

static bool is_pre_bit(int len, int phase) {
	if ((FER_PRE_WIDTH_MIN <= len && len <= FER_PRE_WIDTH_MAX) && (FER_PRE_NEDGE_MIN <= phase && phase <= FER_PRE_NEDGE_MAX)) {
      pre_len = len; pre_phase = phase;
		return true;
    }
	return false;
}

/* returns true if we are inside a start/sync bit. it returns false at
 the rising edge of the next data bit. */
// returns: 0
static int recv_detect_start() {
	// ----____________________________________ (STP_NEDGE:STP_WIDTH) (4:36 @T100)
	// ---_____ || -----___ || ------__ || --______  PRE (7x)
	int result = false;
	static int highCount, allCount, preCounter;
#define lowCount (allCount - highCount)

	if (!(input && (highCount < allCount)))
		++allCount;

	if (input && (highCount + 1 == allCount)) {
		++highCount;
		goto before_phase;
	}

	if (!input)
		goto after_phase;
	else {

		// one -_/bit received. analyzing now
		if (is_pre_bit(allCount, highCount)) {
			++preCounter;
			goto after_pre;
		}

		{
			bool is_long = (allCount > FER_STP_WIDTH_MAX * 2);

			if (is_long) {
				result = -1;
			} else if (is_start_bit(allCount, highCount)) {
				result = 1 + preCounter;
			} else {
				result = -2;
			}
		}
		goto done;
	}

	after_pre: allCount = highCount = 1;
	return result;

	done: preCounter = allCount = highCount = 0;
	after_phase: before_phase: return result;

#undef lowCount
}

static bool dg_detect_longPause() {
	static uint16_t lastFall;
	bool result = false;

	if (input && lastFall > veryLongPauseLow_Len)
		result = true;

	if (input)
		lastFall = 0;
	else
		++lastFall;

	return result;
}

static bool cmd_recv(void) {
	bool result = false;

#if 1
	if (dg_detect_longPause()) {
		//db_puts("LP");
		goto err;
	}
#endif

	// stop counter at zero until start bit is over
	if (POS__NOT_IN_DATA) {
		if (CountWords == 0 && (recv_detect_start() < 1 + 5))
			goto cont;
		//(FIXME: literals)  some pre bits + 1 start bit required
		if (CountWords > 0 && (recv_detect_start() != 1))
			goto cont;
		// only start bit required, no pre bits allowed
		//db_puts("sbc\n");
		// start bit is over here
	}
#define POS__IN_DATA (CountTicks > 0 && CountBits < bitsPerWord)

	if (POS__IN_DATA) {
		store_sample();  // try sampling data
	}

	// advance counters until end_of_line
	if (advance_CmdCounter()) {
		result = true;
		goto succ;
	}

	goto cont;
	err: result = false; //?
	succ: CountTicks = CountBits = CountWords = CountLines = 0;
	cont: return result;

}

static bool advancePrgCounter(void) {
	bool end_of_bit = false, end_of_word = false, end_of_line = false, end_of_frame = false;
    
	precond((CountTicks < bitLen) && (CountBits < bitsPerWord) && (CountWords < FER_PRG_WORD_CT) && (CountLines < FER_PRG_PACK_CT));
 #if TEST_CLOCK_EDGE  
    if (clock_edge) {
      if (CountTicks > bitLen / 2) {
        CountTicks = bitLen;
      }
      clock_edge = false;
    }
 #endif       
	(end_of_bit = ct_incr(CountTicks, bitLen)) && (end_of_word = ct_incr(CountBits, bitsPerWord)) && (end_of_line = ct_incr(CountWords, wordsPerPrgLine))
			&& (end_of_frame = ct_incr(CountLines, linesPerPrg));

	// check for RTC-only frame
	if (!end_of_frame && end_of_line && (CountLines == 1) && FRB_GET_FPR0_IS_RTC_ONLY(dtRecvPrgFrame[FPR_RTC_START_ROW])) {
		CountLines = 0;
		end_of_frame = true;
	}

	// store received words into byte array
	if (!end_of_frame && end_of_word && (CountWords % 2) != 0) {
		recv_decodeByte(&dtRecvPrgFrame[CountLines][CountWords / 2]);
	}

	// when end_frame is reached we return true and all counters should be zero
	postcond(!end_of_frame || (!(CountTicks || CountBits || CountWords || CountLines)));

	return end_of_frame;
}

#define MAX_PRGCALLCOUNT ((FER_PRG_FRAME_WIDTH_T100 * FER_TICK_FREQ_MULT)  + (100 * FER_TICK_FREQ_MULT))

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
	precond(prgCallCount != 0 || !(CountTicks || CountBits || CountWords || CountLines));

	// reset error
	if (prgCallCount++ == 0) {
		error = 0;
	}

#if 0 //FIXME
	// timeout (e.g. signal loss)
	if (prgCallCount > MAX_PRGCALLCOUNT) {
		goto err;
	}
#endif

	// there is no pause inside a prg sequence
	if (dg_detect_longPause()) {
		goto err;
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
	if (advancePrgCounter() != 0) {
		if (error || CountRecvErrors)
			goto err;

		result = 1;
		goto succ;
	}

	goto cont;
	err: error = 1;
	result = -1;
	succ:
	//io_puts(error ? "Pbd\n" :"Pgd\n");
	prgCallCount = 0;
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
	input = fer_get_recvPin();
 
 #if 1   
        if (input != old_input) {
          old_input = input;
          clock_edge = input;
          data_edge = !clock_edge;
          
          if (clock_edge) {
              data_edge_ticks = -1;
          }                           
        }
           ++data_edge_ticks;
  #endif      
	tick_recv_command();
#ifndef FER_RECEIVER_MINIMAL
	tick_recv_programmingFrame();
#endif
}

#endif
