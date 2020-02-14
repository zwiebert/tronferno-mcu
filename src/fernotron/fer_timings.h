/*
 * fer_timings.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_TIMINGS_H_
#define USER_MAIN_FER_TIMINGS_H_

////////////////////////////////////////////////////////////////////////////////
// timing/sizes of RF modulation

// imaginary "data clock" which is "four times preamble frequency"
#define BASE_CLOCK       5000UL  // 5kHz . 200us
#define DEFAULT_DATA_CLOCK_HZ  5000 // 5kHz
#define DEFAULT_DATA_CLOCK_US  200  // 200us (us=1/MHz)

// timings relative to "data clock" above

// PREAMBLE - provides clock signal
// before sending any data we have 7 pre-bits
// /--\__ (2_on + 2_off = 4) * 7
// (200us * 4 * 7)
#define FER_PRE_WIDTH_DCK         4
#define FER_PRE_WIDTH_MAX_DCK    10
#define FER_PRE_WIDTH_MIN_DCK     3
#define FER_PRE_NEDGE_MIN_DCK     1
#define FER_PRE_NEDGE_MAX_DCK     5

// STOP BIT - separates words
// 1 stop bit comes before each preamble and each data word
// /--\__________________ (2_on + 16_off = 18) * 1
// (200us * 18 * 1)
#define FER_STP_WIDTH_DCK        18
#define FER_STP_WIDTH_MIN_DCK    15
#define FER_STP_WIDTH_MAX_DCK    30
#define FER_STP_NEDGE_DCK         2
#define FER_STP_NEDGE_MIN_DCK     1
#define FER_STP_NEDGE_MAX_DCK     6

// DATA BIT - encodes 1 or 0
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

// counts
#define FER_CMD_BIT_CT           10  // 10 bits per word
#define FER_CMD_WORD_CT          12  // 12 words per command
#define FER_CMD_BYTE_CT           (FER_CMD_WORD_CT / 2) // 5 data + 1 checksum byte
#define FER_STP_BIT_CT            (FER_STP_WIDTH_DCK / FER_BIT_WIDTH_DCK)  // 18/6=3 (a stop-bit is 3 times larger than a databit)
#define FER_PRE_BIT_CT            7 // 7 clock periods per preamble
#define FER_PRG_WORD_CT          18   // 18 words per frame-line
#define FER_PRG_BYTE_CT          (FER_PRG_WORD_CT / 2)
#define FER_PRG_PACK_CT          18   // 18 lines per program-frame
#define FER_RTC_PACK_CT           1   // 1 line per rtc-frame

#endif /* USER_MAIN_FER_TIMINGS_H_ */
