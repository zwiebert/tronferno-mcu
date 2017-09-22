// -*- C -*-
#ifndef FER_H
#define FER_H

#include "fer_code.h"

#define FER_TICK_FREQ_MULT             2 // 0.5=5kHz (200us), 1=10kHz (100us), 2=20kHz (50us), 4=40kHz (24us)
#define TICK_HZ                  (10000UL * FER_TICK_FREQ_MULT)  // 10 kHz * MULT
#define TICK_US                  (1000000UL / TICK_HZ)      // 100us / MULT

#define DEFAULT_DATA_CLOCK_US  200
#define DEFAULT_DATA_CLOCK_HZ  5000
#define DEFAULT_DATA_CLOCK_TICKS US_TO_TICKS(DEFAULT_DATA_CLOCK_US)

#define REL_TO_TICKS(rel) (((rel) * DEFAULT_DATA_CLOCK_US) / TICK_US)
#define HUS_TO_TICKS(hus) (((hus) * 100 ) / TICK_US)
#define US_TO_TICKS(us) ((us) / TICK_US)

// timings (relative to "data clock" which is "four times prelude frequency" (usually 200us)).
// before sending any data we have 7 pre-bits 
#define FER_PRE_WIDTH_REL         4
#define FER_PRE_WIDTH_MAX_REL    10
#define FER_PRE_WIDTH_MIN_REL     3 //3.5
#define FER_PRE_NEDGE_MIN_REL     1
#define FER_PRE_NEDGE_MAX_REL     5

// a stop bits separate each data-word from another
// and also the first data word from the pre-bits
#define FER_STP_WIDTH_REL        18
#define FER_STP_WIDTH_MIN_REL    15
#define FER_STP_WIDTH_MAX_REL    30
#define FER_STP_NEDGE_REL         2
#define FER_STP_NEDGE_MIN_REL     1
#define FER_STP_NEDGE_MAX_REL     6 // 5.5

// data bits are represented by two different positions of the negative edge
// 0=short-long  /--\____/
// 1=long-short  /----\__/
#define FER_BIT_WIDTH_REL         6
#define FER_BIT_NEDGE_0_REL       2
#define FER_BIT_NEDGE_1_REL       4
#define FER_BIT_SAMP_POS_REL      3

#define FER_DAT_WORD_WIDTH_US    (FER_STP_WIDTH_US + (1UL * FER_BIT_WIDTH_US * FER_CMD_BIT_CT)) //15600
#define FER_PRG_FRAME_WIDTH_US   (1UL * FER_DAT_WORD_WIDTH_US * FER_PRG_WORD_CT * FER_PRG_PACK_CT)  //5054400

#define FER_PRE_WIDTH       REL_TO_TICKS(FER_PRE_WIDTH_REL)
#define FER_PRE_WIDTH_MIN   REL_TO_TICKS(FER_PRE_WIDTH_MIN_REL)
#define FER_PRE_WIDTH_MAX   REL_TO_TICKS(FER_PRE_WIDTH_MAX_REL)
#define FER_PRE_NEDGE_MIN   REL_TO_TICKS(FER_PRE_NEDGE_MIN_REL)
#define FER_PRE_NEDGE_MAX   REL_TO_TICKS(FER_PRE_NEDGE_MAX_REL)

#define FER_STP_WIDTH       REL_TO_TICKS(FER_STP_WIDTH_REL)
#define FER_STP_WIDTH_MIN   REL_TO_TICKS(FER_STP_WIDTH_MIN_REL)
#define FER_STP_WIDTH_MAX   REL_TO_TICKS(FER_STP_WIDTH_MAX_REL)
#define FER_STP_NEDGE       REL_TO_TICKS(FER_STP_NEDGE_REL)
#define FER_STP_NEDGE_MIN   REL_TO_TICKS(FER_STP_NEDGE_MIN_REL)
#define FER_STP_NEDGE_MAX   REL_TO_TICKS(FER_STP_NEDGE_MAX_REL)

#define FER_BIT_WIDTH       REL_TO_TICKS(FER_BIT_WIDTH_REL)
#define FER_BIT_NEDGE_0     REL_TO_TICKS(FER_BIT_NEDGE_0_REL)
#define FER_BIT_NEDGE_1     REL_TO_TICKS(FER_BIT_NEDGE_1_REL)
#define FER_BIT_SAMP_POS    REL_TO_TICKS(FER_BIT_SAMP_POS_REL)

// sizes
#define FER_CMD_BIT_CT           10  // we have 10 data bits + 1 stop bit
#define FER_CMD_WORD_CT          12
#define FER_CMD_BYTE_CT           (FER_CMD_WORD_CT / 2 - 1)
#define FER_STP_BIT_CT            3  // 3 times larger than a databit (\___/--------------------------\)
#define FER_PRE_BIT_CT            7
#define FER_PRG_WORD_CT          18   // words per line
#define FER_PRG_BYTE_CT          (FER_PRG_WORD_CT / 2)
#define FER_PRG_PACK_CT          18   // lines per frame
#define FER_RTC_PACK_CT           1   // lines per frame

#define bitsPerPause          FER_STP_BIT_CT  // this just means 3 databits would fit into 1 pause/stop-bit
#define bitsPerPre            FER_PRE_BIT_CT
#define bitsPerWord           FER_CMD_BIT_CT
#define wordsPerCmdPacket     FER_CMD_WORD_CT
#define bytesPerCmdPacket     (FER_CMD_WORD_CT / 2)
#define linesPerPrg           FER_PRG_PACK_CT
#define bytesPerPrgLine       FER_PRG_BYTE_CT
#define wordsPerPrgLine       FER_PRG_WORD_CT

#define bitLen               FER_BIT_WIDTH
#define shortPositive_Len        FER_BIT_NEDGE_0
#define longPositive_Len         FER_BIT_NEDGE_1
#define dtSample_Pos         FER_BIT_SAMP_POS
#define pre_Len              FER_PRE_WIDTH

#define pause_Len            FER_STP_WIDTH
#define pauseHigh_Len        FER_STP_NEDGE

typedef uint8_t *ferCmdBuf_type;

// receiver ///////////////////////////
bool fer_get_recvPin();

extern uint16_t CountRecvErrors;  // FIXME: get rid of it

ferCmdBuf_type get_recvCmdBuf(void);
uint8_t *get_recvPrgBufLine(uint8_t line);
extern uint8_t dtRecvPrgFrame[linesPerPrg][bytesPerPrgLine];

extern volatile bool has_cmdReceived, has_prgReceived;
void tick_ferReceiver(void);
extern bool fer_prg_verfiy_checksums(uint8_t dg[linesPerPrg][bytesPerPrgLine], uint8_t checksum);
bool fer_cmd_verify_checksum(ferCmdBuf_type dg, uint8_t *checksum_out);

// transmitter //////////////////////////
void fer_put_sendPin(bool dat);

extern uint8_t dtSendPrgFrame[linesPerPrg][bytesPerPrgLine];
ferCmdBuf_type get_sendCmdBuf(void);
uint8_t *get_sendPrgBufLine(uint8_t line);

extern volatile bool is_sendCmdPending, is_sendPrgPending;
void tick_ferSender(void);
bool fer_send_prg(fer_sender_basic *fsb);
bool fer_send_cmd(fer_sender_basic *fsb);
extern uint8_t fer_prg_create_checksums(uint8_t dg[linesPerPrg][bytesPerPrgLine], uint8_t checksum);


void init_prgPacket(uint8_t[linesPerPrg][bytesPerPrgLine]);
void write_rtc(uint8_t d[FPR_RTC_WIDTH], bool rtc_only);
void write_lastline(fer_sender_basic *fsb, uint8_t d[FPR_ASTRO_WIDTH]);

// diagnostic output
void frb_printPacket(const uint8_t *dg);
void fpr_printPrgPacketInfo(uint8_t d[linesPerPrg][bytesPerPrgLine], bool rtc_only);

#endif // FER_H
