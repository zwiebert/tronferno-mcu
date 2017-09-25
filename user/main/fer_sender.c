#include "common.h"
#include "fer.h"
#include "counter.h"
#include "inout.h"
#include "utils.h"


#define IS_EO_BIT (CountTicks == 0)
#define IS_EO_WORD (IS_EO_BIT && CountBits == 0)
#define IS_EO_LINE (IS_EO_WORD && CountWords == 0)
#define IS_EO_FRAME (IS_EO_LINE && CountLines == 0)


volatile bool is_sendCmdPending, is_sendPrgPending;

static uint8_t CountTicks, CountBits, CountWords, CountLines;

static uint16_t dtSendBuf;
static uint8_t dtSendCmd[bytesPerCmdPacket];
uint8_t dtSendPrgFrame[linesPerPrg][bytesPerPrgLine];

static uint16_t dtWordOut;  // holds current data word
static bool dtLineOut;   // output line

ferCmdBuf_type ICACHE_FLASH_ATTR get_sendCmdBuf(void) {
	return dtSendCmd;
}
uint8_t * ICACHE_FLASH_ATTR get_sendPrgBufLine(uint8_t idx) {
	return dtSendPrgFrame[idx];
}

uint8_t
fer_make_cmdPacket(const uint8_t *src, uint8_t *dst) {
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
	io_puts("S:");
	frb_printPacket(dtSendCmd);
	is_sendCmdPending = true;
	return true;
}

bool ICACHE_FLASH_ATTR fer_send_prg(fer_sender_basic *fsb) {
	if (is_sendCmdPending || is_sendPrgPending)
		return false;

	uint8_t cmd_checksum = fer_make_cmdPacket(fsb->data, dtSendCmd);
	fer_prg_create_checksums(dtSendPrgFrame, cmd_checksum);

	io_puts("S:");
	frb_printPacket(dtSendCmd);

	is_sendCmdPending = true;
	is_sendPrgPending = true;
	return true;
}

/////////////////////////// interrupt code //////////////////////


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

// sets output line according to current bit in data word
// after the word is sent, the line is set to zero while counting the pause-"bits"
static void updateLine() {
	dtWordOut = dtSendBuf;
	dtLineOut = ((CountTicks < shortPositive_Len && CountBits < bitsPerWord + 1)  // only in data word
	|| (CountTicks < longPositive_Len && CountBits < bitsPerWord   // only in data word
	&& !(dtWordOut & (1 << CountBits))));
}

static bool advanceCmdCounter() {
	bool end_of_bit = false, end_of_stop = false, end_of_line = false;

	(end_of_bit = ct_incr(CountTicks, bitLen)) && (end_of_stop = ct_incr(CountBits, bitsPerWord + bitsPerPause))
			&& (end_of_line = ct_incr(CountWords, wordsPerCmdPacket));

	if (!end_of_line // not done?
	&& (end_of_stop // next word into buffer
	|| (CountTicks == 1 && !CountBits && !CountWords)))  // very first word into buffer
			{
		dtSendBuf = make_Word(dtSendCmd[CountWords / 2], CountWords);
	}
	return end_of_line;
}

static bool sendCmd() {
	static int state;
	enum {
		stPre, stStop, stData, stDone
	};

	switch (state) {

	case stDone:
		state = stPre;
		// fall through
	case stPre:
		updateLinePre();
		if (advancePreCounter())
			state = stStop;
		break;

	case stStop:
		updateLineStop();
		if (advanceStopCounter())
			state = stData;
		break;

	case stData:
		updateLine();
		if (advanceCmdCounter())
			state = stDone;
		break;
	}

	return state == stDone;
}

static bool advancePrgCounter(void) {
	bool end_of_bit = false, end_of_word = false, end_of_line = false, end_of_frame = false;

	precond((CountTicks < bitLen) && (CountBits < bitsPerWord) && (CountWords < FER_PRG_WORD_CT) && (CountLines < FER_PRG_PACK_CT));

	(end_of_bit = ct_incr(CountTicks, bitLen)) && (end_of_word = ct_incr(CountBits, bitsPerWord + bitsPerPause))
			&& (end_of_line = ct_incr(CountWords, wordsPerPrgLine)) && (end_of_frame = ct_incr(CountLines, (linesPerPrg)));

	// an rtc frame is only one line long
	if (!end_of_frame && end_of_line && CountLines == 1 && FRB_GET_FPR0_IS_RTC_ONLY(dtSendPrgFrame[FPR_RTC_START_ROW])) {
		end_of_frame = true;
	}

	// copy data into sendBuf
	if (!end_of_frame) {
		if (end_of_word  // next word into buffer
		|| (CountTicks == 1 && !CountBits && !CountWords))  // very first word into buffer)
			dtSendBuf = make_Word(dtSendPrgFrame[CountLines][CountWords / 2], CountWords);
	}

	// when end_frame is reached we return true and all counters should be zero
	postcond(!end_of_frame || (!(CountTicks || CountBits || CountWords || CountLines)));

	return end_of_frame;
}

static bool sendPrg() {
	updateLine();
	return advancePrgCounter();
}


static void tick_send_command() {
	if (is_sendCmdPending) {
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
}

static void tick_send_programmingFrame() {

	if (!is_sendCmdPending && is_sendPrgPending) {
		bool done = sendPrg();
		fer_put_sendPin(dtLineOut);

		if (done) {
			fer_put_sendPin(0);
			is_sendPrgPending = false;
			init_counter();
		}
	}
}

void tick_ferSender(void) {
	tick_send_command();
	tick_send_programmingFrame();
}
