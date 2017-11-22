// -*- C -*-
#ifndef FER_H
#define FER_H

#include "fer_code.h"

////////////////////////////////////////////////////////////////////////////////
// timing/sizes for RF modulation

#define BASE_CLOCK       5000UL  // 5kHz . 200us
#define INTR_TICK_FREQ_MULT 4    // 1=200us, 2=100us, 4=50us, 8=25us
#define FER_TICK_FREQ_MULT       (INTR_TICK_FREQ_MULT / 2)  // old-code 100us relative multiplier: 0.5=5kHz (200us), 1=10kHz (100us), 2=20kHz (50us), 4=40kHz (25us)
#define TICK_FREQ_HZ             (BASE_CLOCK * INTR_TICK_FREQ_MULT) 
#define TICK_PERIOD_US           (1000000UL / TICK_FREQ_HZ)
#define TICK_PERIOD_MS           (TICK_PERIOD_US * 1000UL)

#define DEFAULT_DATA_CLOCK_US  200
#define DEFAULT_DATA_CLOCK_HZ  5000
#define DEFAULT_DATA_CLOCK_TICKS US_TO_TICKS(DEFAULT_DATA_CLOCK_US)

#define DATA_CLOCK_TO_TICKS(rel) (((rel) * DEFAULT_DATA_CLOCK_US) / TICK_PERIOD_US)
//#define REL_TO_TICKS(rel) ((rel) * INTR_TICK_FREQ_MULT)

#define HUS_TO_TICKS(hus) (((hus) * 100 ) / TICK_PERIOD_US)
#define US_TO_TICKS(us) ((us) / TICK_PERIOD_US)
#define MS_TO_TICKS(ms) ((ms) / TICK_PERIOD_MS)

// timings (relative to an imaginary "data clock" which is "four times preamble frequency" (usually 200us)).
// 1 stop bit comes before each preamble and each data word
// /--\__________________ (2_on + 16_off = 18) * 1
// (200us * 18 * 1)
#define FER_STP_WIDTH_DCK        18
#define FER_STP_WIDTH_MIN_DCK    15
#define FER_STP_WIDTH_MAX_DCK    30
#define FER_STP_NEDGE_DCK         2
#define FER_STP_NEDGE_MIN_DCK     1
#define FER_STP_NEDGE_MAX_DCK     6

// before sending any data we have 7 pre-bits
// /--\__ (2_on + 2_off = 4) * 7 
// (200us * 4 * 7)
#define FER_PRE_WIDTH_DCK         4
#define FER_PRE_WIDTH_MAX_DCK    10
#define FER_PRE_WIDTH_MIN_DCK     3
#define FER_PRE_NEDGE_MIN_DCK     1
#define FER_PRE_NEDGE_MAX_DCK     5

// data bits are represented by two different positions of the negative edge (on-off)
// 1=short-long  /--\____  (2_on, 4_off)
// 0=long-short  /----\__  (4_on, 2_off)
// 10 bits makes one data-word
// (200us * 6 * 10)
#define FER_BIT_WIDTH_DCK         6
#define FER_BIT_SHORT_DCK         2
#define FER_BIT_LONG_DCK          4
#define FER_BIT_SAMP_POS_DCK      (FER_BIT_WIDTH_DCK / 2) // for receiving: look if negative edge lies before or after

// -----stop------- ---------pre--    ----stop------     --data-word---     ----stop------     --data-word---     ----stop------      ...  + -- last data-word---
// (200us * 18 * 1) + (200us * 4 * 7) + (200us * 18 * 1) + (200us * 6 * 10) + (200us * 18 * 1) + (200us * 6 * 10) + (200us * 18 * 1)  ...  + (200us * 6 * 10)

#define FER_DAT_WORD_WIDTH_DCK    (FER_STP_WIDTH_DCK + (1UL * FER_BIT_WIDTH_DCK * FER_CMD_BIT_CT))
#define FER_PRG_FRAME_WIDTH_DCK   (1UL * FER_DAT_WORD_WIDTH_DCK * FER_PRG_WORD_CT * FER_PRG_PACK_CT)

//  the same timings relative to ticks of interrupt frequency
#define FER_PRE_WIDTH_TCK       DATA_CLOCK_TO_TICKS(FER_PRE_WIDTH_DCK)
#define FER_PRE_WIDTH_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_WIDTH_MIN_DCK)
#define FER_PRE_WIDTH_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_WIDTH_MAX_DCK)
#define FER_PRE_NEDGE_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_NEDGE_MIN_DCK)
#define FER_PRE_NEDGE_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_PRE_NEDGE_MAX_DCK)

#define FER_STP_WIDTH_TCK       DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_DCK)
#define FER_STP_WIDTH_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MIN_DCK)
#define FER_STP_WIDTH_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_STP_WIDTH_MAX_DCK)
#define FER_STP_NEDGE_TCK       DATA_CLOCK_TO_TICKS(FER_STP_NEDGE_DCK)
#define FER_STP_NEDGE_MIN_TCK   DATA_CLOCK_TO_TICKS(FER_STP_NEDGE_MIN_DCK)
#define FER_STP_NEDGE_MAX_TCK   DATA_CLOCK_TO_TICKS(FER_STP_NEDGE_MAX_DCK)

#define FER_BIT_WIDTH_TCK       DATA_CLOCK_TO_TICKS(FER_BIT_WIDTH_DCK)
#define FER_BIT_SHORT_TCK       DATA_CLOCK_TO_TICKS(FER_BIT_SHORT_DCK)
#define FER_BIT_LONG_TCK        DATA_CLOCK_TO_TICKS(FER_BIT_LONG_DCK)
#define FER_BIT_SAMP_POS_TCK    DATA_CLOCK_TO_TICKS(FER_BIT_SAMP_POS_DCK)

// counts
#define FER_CMD_BIT_CT           10  // we have 10 data bits (+ 1 stop bit)
#define FER_CMD_WORD_CT          12
#define FER_CMD_BYTE_CT           (FER_CMD_WORD_CT / 2 - 1)
#define FER_STP_BIT_CT            (FER_STP_WIDTH_DCK / FER_BIT_WIDTH_DCK)  // 18/6=3 (a stop-bit is 3 times larger than a databit)
#define FER_PRE_BIT_CT            7
#define FER_PRG_WORD_CT          18   // words per line
#define FER_PRG_BYTE_CT          (FER_PRG_WORD_CT / 2)
#define FER_PRG_PACK_CT          18   // lines per frame
#define FER_RTC_PACK_CT           1   // lines per frame

// aliases
#define bitsPerPause          FER_STP_BIT_CT  // this just means 3 databits would fit into 1 pause/stop-bit. helps with counting them out
#define bitsPerPre            FER_PRE_BIT_CT
#define bitsPerWord           FER_CMD_BIT_CT
#define wordsPerCmdPacket     FER_CMD_WORD_CT
#define bytesPerCmdPacket     (FER_CMD_WORD_CT / 2)
#define linesPerPrg           FER_PRG_PACK_CT
#define bytesPerPrgLine       FER_PRG_BYTE_CT
#define wordsPerPrgLine       FER_PRG_WORD_CT

#define FER_SENDER_DCK   // use data clock instead of tick clock for fer_sender.c

////////////////////////////////////////////////////////////////////////////////////
// implementation interface

typedef enum {
	fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM
} fer_error;

typedef enum {
	MSG_TYPE_NONE, MSG_TYPE_PLAIN, MSG_TYPE_RTC, MSG_TYPE_TIMER
} fmsg_type;

typedef struct fer_msg {
	uint8_t cmd[bytesPerCmdPacket];
	uint8_t rtc[bytesPerPrgLine];
	uint8_t wdtimer[FPR_TIMER_HEIGHT][bytesPerPrgLine];
	uint8_t astro[FPR_ASTRO_HEIGHT][bytesPerPrgLine];
	uint8_t last[bytesPerPrgLine];
}__attribute__((__packed__)) fer_msg;

#define fmsg_get_data(msg) ((uint8_t(*)[bytesPerPrgLine])(msg)->rtc)
typedef uint8_t(*fmsg_data)[bytesPerPrgLine];

#define BYTES_MSG_PLAIN  bytesPerCmdPacket
#define BYTES_MSG_RTC    (bytesPerCmdPacket + bytesPerPrgLine * 1)
#define BYTES_MSG_TIMER  (bytesPerCmdPacket + bytesPerPrgLine * linesPerPrg)

extern struct fer_msg message_buffer;
#define rxmsg (&message_buffer)
#define txmsg (&message_buffer)
#define rxbuf ((uint8_t *)rxmsg)
#define txbuf ((uint8_t *)txmsg)
#define rxdat fmsg_get_data(rxmsg)
#define txdat fmsg_get_data(txmsg)

// receiver ///////////////////////////
bool mcu_get_rxPin();

uint8_t *get_recvPrgBufLine(uint8_t line);

extern volatile bool has_cmdReceived;
extern volatile bool has_prgReceived;

extern volatile uint8_t MessageReceived;

void frx_tick(void);
void frx_clear(void); // call it after received data buffers has been processed by main thread
bool recv_lockBuffer(bool enableLock);  // blocks receiver access to shared buffer

// transmitter //////////////////////////
void mcu_put_txPin(bool dat);

extern uint8_t dtSendPrgFrame[linesPerPrg][bytesPerPrgLine];

extern volatile bool is_sendMsgPending;
void ftx_tick(void);
bool fer_send_prg(fer_sender_basic *fsb);
bool fer_send_cmd(fer_sender_basic *fsb);

void fmsg_create_checksums(struct fer_msg *m, fmsg_type t);
bool fmsg_verify_checksums(const struct fer_msg *m, fmsg_type t);
void fmsg_init_data(fer_msg *msg);
void fmsg_write_rtc(fer_msg *msg, bool rtc_only);
void fmsg_write_flags(fer_msg *msg, uint8_t flags, uint8_t mask);
void fmsg_write_wtimer(fer_msg *msg, const uint8_t *wtimer_data);
void fmsg_write_dtimer(fer_msg *msg, const uint8_t *dtimer_data);
void fmsg_write_astro(fer_msg *msg, int mint_offset);
void fmsg_write_lastline(fer_msg *msg, fer_sender_basic *fsb);
void fmsg_print(const fer_msg *msg, fmsg_type t);
// diagnostic output
void frb_printPacket(const uint8_t *dg);
void fpr_printPrgPacketInfo(uint8_t d[linesPerPrg][bytesPerPrgLine], bool rtc_only);
void fer_printData(const uint8_t *cmd, uint8_t prg[linesPerPrg][bytesPerPrgLine]);
#endif // FER_H
