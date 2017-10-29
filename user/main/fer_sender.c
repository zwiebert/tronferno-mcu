#include "common.h"
#include "fer.h"
#include "counter.h"
#include "inout.h"
#include "utils.h"
#include "config.h"

#define USE_MACROS 1

#define IS_EO_BIT (CountTicks == 0)
#define IS_EO_WORD (IS_EO_BIT && CountBits == 0)
#define IS_EO_LINE (IS_EO_WORD && CountWords == 0)
#define IS_EO_FRAME (IS_EO_LINE && CountLines == 0)


volatile bool is_sendCmdPending, is_sendPrgPending;

static uint8_t CountTicks, CountBits, CountWords, CountLines;

static uint16_t dtSendBuf;
static uint8_t dtSendCmd[bytesPerCmdPacket];
uint8_t dtSendPrgFrame[linesPerPrg][bytesPerPrgLine];

static bool dtLineOut;   // output line

ferCmdBuf_type ICACHE_FLASH_ATTR get_sendCmdBuf(void) {
	return dtSendCmd;
}
uint8_t * ICACHE_FLASH_ATTR get_sendPrgBufLine(uint8_t idx) {
	return dtSendPrgFrame[idx];
}

uint8_t ICACHE_FLASH_ATTR fer_make_cmdPacket(const uint8_t *src, uint8_t *dst) {
  int i;
  uint8_t check_sum = 0;

  for (i=0; i < (bytesPerCmdPacket - 1); ++i) {
    dst[i] = src[i];
    check_sum += src[i];
  }

  return dst[i] = check_sum;
}


bool ICACHE_FLASH_ATTR fer_send_cmd(fer_sender_basic *fsb) {
	if (is_sendCmdPending || is_sendPrgPending)
		return false;

	fer_make_cmdPacket(fsb->data, dtSendCmd);
	if (C.app_verboseOutput >= vrb1)
		io_puts("S:"), frb_printPacket(dtSendCmd);
	is_sendCmdPending = true;
	return true;
}

bool ICACHE_FLASH_ATTR fer_send_prg(fer_sender_basic *fsb) {
	if (is_sendCmdPending || is_sendPrgPending)
		return false;

	uint8_t cmd_checksum = fer_make_cmdPacket(fsb->data, dtSendCmd);
	fer_prg_create_checksums(dtSendPrgFrame, cmd_checksum);

if (C.app_verboseOutput >= vrb1)
	io_puts("S:"), fer_printData(dtSendCmd, C.app_verboseOutput >= vrb2 ? dtSendPrgFrame : 0);

	is_sendCmdPending = true;
	is_sendPrgPending = true;
	return true;
}

/////////////////////////// interrupt code //////////////////////


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

#if USE_MACROS
#define make_Word fer_add_word_parity
#define init_counter() (CountTicks = CountBits = CountWords = CountLines = 0)
#define updateLinePre() (dtLineOut = (CountTicks < shortPositive_Len))
#define advancePreCounter() (ct_incr(CountTicks, pre_Len) && ct_incr(CountBits, bitsPerPre))
#define advanceStopCounter() (ct_incr(CountTicks, bitLen) && ct_incr(CountBits, bitsPerPause))
#define updateLineStop() (dtLineOut = (CountTicks < pauseHigh_Len) && (CountBits == 0))
#else
static uint16_t
make_Word(uint8_t val, uint8_t wordCounter)
{
  uint16_t result = fer_add_word_parity(val, wordCounter);
  return result;
}

static void init_counter() {
  CountTicks = CountBits = CountWords = CountLines = 0;
}

static void updateLinePre() {
	dtLineOut = (CountTicks < shortPositive_Len);
}

static bool advancePreCounter() {
	return (ct_incr(CountTicks, pre_Len) && ct_incr(CountBits, bitsPerPre));
}

static bool advanceStopCounter() {
	return (ct_incr(CountTicks, bitLen) && ct_incr(CountBits, bitsPerPause));
}

static void updateLineStop() {
	dtLineOut = (CountTicks < pauseHigh_Len) && (CountBits == 0);
}
#endif

// sets output line according to current bit in data word
// after the word is sent, the line is set to zero while counting the pause-"bits"
static void updateLine() {
	dtLineOut = ((CountTicks < shortPositive_Len && CountBits < bitsPerWord + 1)  // pShort: in both data word and stop bit (+1)
	|| (CountTicks < longPositive_Len && CountBits < bitsPerWord  && !(dtSendBuf & (1 << CountBits))));  // pLong: only in data word
	
}



static bool sendCmd() {
  static bool end_of_preamble, end_of_stop;
  
  if (!end_of_preamble) {
    // send preamble
    updateLinePre();
    end_of_preamble = advancePreCounter();
    } else 	if (!end_of_stop) {
      //send single stop bit after preamble
    updateLineStop();
    if ((end_of_stop = advanceStopCounter()))
       dtSendBuf = make_Word(dtSendCmd[0], 0); // load first word into send buffer
    
    } else {
      // send data words + stop bits
    updateLine();
    // counting ticks until end_of_frame
    if (ct_incr(CountTicks, bitLen)) {
      // bit sent
      if ((ct_incr(CountBits, bitsPerWord + bitsPerPause))) {
        // word + stop sent
        if (ct_incr(CountWords, wordsPerCmdPacket)) {
          // line sent
          end_of_stop = end_of_preamble = false;
          return true; // done
        } else {
          dtSendBuf = make_Word(dtSendCmd[CountWords / 2], CountWords); // load next word
        }
      }
    }
  }
  return false; // continue
}

static bool sendPrg() {
  bool end_of_word = false;
  updateLine();
  
  // counting ticks until end_of_frame
  if (ct_incr(CountTicks, bitLen)) {
    // bit sent
    if ((end_of_word = ct_incr(CountBits, bitsPerWord + bitsPerPause))) {
      // word sent
      if (ct_incr(CountWords, wordsPerPrgLine)) {
        // line sent
        if ((CountLines == 0 &&  FRB_GET_FPR0_IS_RTC_ONLY(dtSendPrgFrame[FPR_RTC_START_ROW])) || ct_incr(CountLines, linesPerPrg)) {
          // frame sent
          return true;
        }     
      }
    }
  }
  
  if (end_of_word) //|| (CountTicks == 1 && !CountBits && !CountWords))
  dtSendBuf = make_Word(dtSendPrgFrame[CountLines][CountWords / 2], CountWords);
  
  return false; //advancePrgCounter();
}

static void tick_send_command() {

	bool done = sendCmd();
	fer_put_sendPin(dtLineOut);

	if (done) {
		is_sendCmdPending = false;
		init_counter();
		if (is_sendPrgPending) {
			dtSendBuf = make_Word(dtSendPrgFrame[0][0], 0);
		} else {
			fer_put_sendPin(0);
		}
	}
}

static void tick_send_programmingFrame() {
	bool done = sendPrg();
	fer_put_sendPin(dtLineOut);

	if (done) {
		fer_put_sendPin(0);
		is_sendPrgPending = false;
		init_counter();
	}
}

void tick_ferSender(void) {

	if (is_sendCmdPending) {
		tick_send_command();
	} else if (is_sendPrgPending) {
		tick_send_programmingFrame();
	}
}
