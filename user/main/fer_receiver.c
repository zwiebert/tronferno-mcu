#include <stdlib.h>

#include "common.h"
#include "counter.h"
#include "fer.h"

#include "utils.h"
#include "fer_code.h"
#include "config.h"

#ifndef FER_RECEIVER

bool ICACHE_FLASH_ATTR recv_lockBuffer(bool enableLock) {
	return true;
}

#else

#define bitLen               FER_BIT_WIDTH_TCK
// bit is 1, if data edge comes before sample position (/SHORT\..long../)
// bit is 0, if data edge comes after sample position  (/..LONG..\short/)
#define SAMPLE_BIT ((pTicks < FER_BIT_SAMP_POS_TCK))


static volatile bool requestLock;
static volatile bool isLocked;

bool ICACHE_FLASH_ATTR recv_lockBuffer(bool enableLock) {
#if BUFFER_SHARING
	if (enableLock) {
		requestLock = true;
		do {
			mcu_delay_us(100);
		} while (!isLocked && recvTick == C.app_recv);
	} else {
		requestLock = false;
	}
#endif
	return true;
}

/////////////////////////// interrupt code //////////////////////
#define IS_P_INPUT (input == true)
#define IS_N_INPUT (input == false)
#define veryLongPauseLow_Len (2 * FER_STP_WIDTH_MAX_TCK)
#define fuzCmp(a,b) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * 2))
#define fuzzCmp(a,b,n) (abs((a)-(b)) < (FER_TICK_FREQ_MULT * (n)))

#define pEdge (input_edge > 0)
#define nEdge (input_edge < 0)
#define clockEdge pEdge
#define dataEdge nEdge

#define MAX_PRG_TICK_COUNT (DATA_CLOCK_TO_TICKS(FER_PRG_FRAME_WIDTH_DCK) * 2) // FIXME

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


volatile uint8_t MessageReceived;
// counters
static uint8_t CountTicks, CountBits, CountWords;

// buffer to store received RF data
static uint16_t dtRecvBuffer[2];

#if BUFFER_SHARING
struct fer_msg *rbuf = &message_buffer;
#else
static struct fer_msg rmsg;
struct fer_msg *rbuf = &rmsg;
#endif


#define getCurrentByteAddr() (currentByteAddr + 0)
static uint16_t received_byte_ct;
static uint8_t *currentByteAddr;
static uint16_t bytesToReceive;
#define getBytesToReceive() (bytesToReceive + 0)
#define hasAllBytesReceived() (bytesToReceive <= received_byte_ct)

static void incrCurrentByteAddr(void) {
	 ++currentByteAddr;
	++received_byte_ct;
}

ferCmdBuf_type ICACHE_FLASH_ATTR get_recvCmdBuf(void) {
	return rbuf->cmd;
}

uint8_t * ICACHE_FLASH_ATTR get_recvPrgBufLine(uint8_t idx) {
	return rbuf->rtc + idx * bytesPerPrgLine;
}

static void init_counter() {
	CountTicks = CountBits = CountWords = 0;
}

/* "calculate 2bit parity value for DATA_BYTE according to POS" */
static uint8_t fer_get_word_parity(uint8_t data_byte, uint8_t pos) {
	uint8_t result;
	bool is_even = is_bits_even(data_byte);

	result = ((pos & 1)) ? (is_even ? 3 : 1) : (is_even ? 0 : 2);

	return result;
}

/* "extend DATA_BYTE with parity according to POS" */
static uint16_t fer_add_word_parity(uint8_t data_byte, int pos) {
	uint16_t result = (data_byte | (((uint16_t) fer_get_word_parity(data_byte, pos)) << 8));
	return result;
}

/* "return t if parity is even and position matches parity bit \ 1/3
 on even positions and 0,2 on odd positions)" */
static bool fer_word_parity_p(uint16_t word, uint8_t pos) {
	bool result = fer_add_word_parity((word & 0xff), pos) == word;
	return result;
}

static fer_errors fer_extract_Byte(const uint16_t *src, uint8_t *dst) {
#if 1
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

static fer_errors fer_verify_cmd(const uint8_t *dg) {
	int i;
	uint8_t checksum = 0;

	for (i = 0; i < bytesPerCmdPacket - 1; ++i) {
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
	return ((FER_STP_WIDTH_MIN_TCK <= len && len <= FER_STP_WIDTH_MAX_TCK) && (FER_STP_NEDGE_MIN_TCK <= nedge && nedge <= FER_STP_NEDGE_MAX_TCK));
}

static bool is_pre_bit(unsigned len, unsigned nedge) {
	return ((FER_PRE_WIDTH_MIN_TCK <= len && len <= FER_PRE_WIDTH_MAX_TCK) && (FER_PRE_NEDGE_MIN_TCK <= nedge && nedge <= FER_PRE_NEDGE_MAX_TCK));
}

static int8_t preBits;

static bool wait_and_sample(void) {

	if (POS__NOT_IN_DATA) {

		if (pEdge) {

			if (preBits < FER_PRE_BIT_CT) {

				// wait until preamble is over
				if (is_pre_bit(aTicks, pTicks) && ++preBits) {
					return false;
				}

			} else {

				// wait until stopBit is over
				if (!is_stopBit(aTicks, pTicks))
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

static bool recvMessage(void) {

	if (wait_and_sample()) {
		if (ct_incr(CountTicks, bitLen)) {
			if (ct_incr(CountBits, bitsPerWord)) {
				// word complete
				if ((CountWords & 1) == 1) {
					// word pair complete
					recv_decodeByte(getCurrentByteAddr());
					incrCurrentByteAddr();
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



void fer_recvClearAll(void) {
	init_counter();
	preBits = 0;
	error = 0;
	MessageReceived = 0;
	received_byte_ct = 0;
	currentByteAddr = &rbuf->cmd[0];
	bytesToReceive = BYTES_MSG_PLAIN;

}

static void tickRecvMessage() {
	if (recvMessage()) {

		switch (getBytesToReceive()) {

		case BYTES_MSG_PLAIN:
			if ((!error && fer_OK == fer_verify_cmd(rbuf->cmd))) {
				if (FRB_GET_CMD(rbuf->cmd) == fer_cmd_Program && FRB_MODEL_IS_CENTRAL(rbuf->cmd)) {
					bytesToReceive = BYTES_MSG_RTC;
				} else {
					MessageReceived = MSG_TYPE_PLAIN;
				}
			} else {
				fer_recvClearAll();
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



void tick_ferReceiver() {

	// sample input pin and detect input edge
	input = fer_get_recvPin();
	input_edge = (old_input == input) ? 0 : (IS_P_INPUT ? +1 : -1);
	old_input = input;

	// receive and decode input
	if (!(MessageReceived || is_sendMsgPending || isLocked)) {

		if ((prgTickCount && !--prgTickCount) || (IS_P_INPUT && nTicks > veryLongPauseLow_Len)) {
			fer_recvClearAll();
		}

		tickRecvMessage();

	}

	if (requestLock) {
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
