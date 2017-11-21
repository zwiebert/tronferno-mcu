#include "common.h"
#include "fer.h"
#include "counter.h"
#include "inout.h"
#include "utils.h"
#include "config.h"

volatile bool is_sendMsgPending;
volatile uint16_t wordsToSend;

#if BUFFER_SHARING
struct fer_msg *tbuf = &message_buffer;
#else
static struct fer_msg tmsg;
struct fer_msg *tbuf = &tmsg;
#endif


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
	if (is_sendMsgPending)
		return false;
	if (recv_lockBuffer(true)) {
	fer_make_cmdPacket(fsb->data, tbuf->cmd);
	if (C.app_verboseOutput >= vrb1)
		io_puts("S:"), frb_printPacket(tbuf->cmd);
	wordsToSend = 2 * BYTES_MSG_PLAIN;

	is_sendMsgPending = true;

	recv_lockBuffer(false);
	return true;
	} else {
		return false;
	}
}


bool ICACHE_FLASH_ATTR fer_send_prg(fer_sender_basic *fsb) {
	if (is_sendMsgPending)
		return false;

	uint8_t cmd_checksum = fer_make_cmdPacket(fsb->data, tbuf->cmd);
	fer_prg_create_checksums(getMsgData(tbuf), cmd_checksum);

if (C.app_verboseOutput >= vrb1)
	io_puts("S:"), fer_printData(tbuf->cmd, C.app_verboseOutput >= vrb2 ? getMsgData(tbuf) : 0);

	wordsToSend = 2 * (FRB_GET_FPR0_IS_RTC_ONLY(tbuf->rtc) ? BYTES_MSG_RTC : BYTES_MSG_TIMER);
	is_sendMsgPending = true;
	return true;
}

// buffer is shared between rx and tx
// ... interrupt code is now in one file too
#if 0
static uint8_t CountTicks, CountBits;
static uint16_t CountWords;
static uint16_t dtSendBuf;
static bool dtLineOut;   // output line

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


#define make_Word fer_add_word_parity
#define init_counter() (CountTicks = CountBits = CountWords = 0)
#define updateLinePre() (dtLineOut = (CountTicks < shortPositive_Len))
#define advancePreCounter() (ct_incr(CountTicks, pre_Len) && ct_incr(CountBits, bitsPerPre))
#define advanceStopCounter() (ct_incr(CountTicks, bitLen) && ct_incr(CountBits, bitsPerPause))
#define updateLineStop() (dtLineOut = (CountTicks < pauseHigh_Len) && (CountBits == 0))


// sets output line according to current bit in data word
// a stop bit is sent in CountBits 0 .. 2
// a data word is sent in CountBits 3 .. 10
static void updateLine() {
	int bit = CountBits - bitsPerPause;

    if (bit < 0) {  // in stop bit (CountBits 0 .. 2)
    	dtLineOut = CountBits == 0 && CountTicks < shortPositive_Len;
    } else { // in data word
    	dtLineOut = CountTicks < shortPositive_Len || (CountTicks < longPositive_Len && !(dtSendBuf & (1 << bit)));
    }
}



static bool sendMsg() {
  static bool end_of_preamble, end_of_stop;

  if (!end_of_stop) {
        //send single stop bit before preamble
      updateLineStop();
      end_of_stop = advanceStopCounter();

      } else  if (!end_of_preamble) {
    // send preamble
    updateLinePre();
    if ((end_of_preamble = advancePreCounter())) {
       dtSendBuf = make_Word(tbuf->cmd[0], 0); // load first word into send buffer
    }
    } else 	{
      // send data words + stop bits
    updateLine();
    // counting ticks until end_of_frame
    if (ct_incr(CountTicks, bitLen)) {
      // bit sent
      if ((ct_incr(CountBits, bitsPerWord + bitsPerPause))) {
        // word + stop sent
        if (ct_incr(CountWords, wordsToSend)) {
          // line sent
          end_of_stop = end_of_preamble = false;
          return true; // done
        } else {
          dtSendBuf = make_Word(tbuf->cmd[CountWords / 2], CountWords); // load next word
        }
      }
    }
  }
  return false; // continue
}



static void tick_send_command() {

	bool done = sendMsg();
	fer_put_sendPin(dtLineOut);

	if (done) {
		is_sendMsgPending = false;
		init_counter();
		fer_put_sendPin(0);

	}
}



void tick_ferSender(void) {

	if (is_sendMsgPending) {
		tick_send_command();
	}
}
#endif


