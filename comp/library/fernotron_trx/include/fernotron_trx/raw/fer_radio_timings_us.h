/**
 * \file   fernotron_trx/raw/fer_radio_timings_us.h
 * \brief  RF message timings for Fernotron
 * \author bertw
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// timing/sizes of RF modulation

// counts
constexpr unsigned FER_CMD_BIT_CT = 10;  ///< 10 bits per word
constexpr unsigned FER_WORD_PER_BYTE_CT = 2;  ///< each data byte is sent twice
constexpr unsigned FER_STP_BIT_CT = 3; ///< (a stop-bit is 3 times larger than a databit)
constexpr unsigned FER_PRE_BIT_CT = 7; ///< 7 clock periods per preamble

/**
 * \verbatim
 * INIT - allows AGC of receiver to adjust on the signal. So the signal is better received at the beginning.
 * it seems, not all transmitters do this (see it on 2431, but not on 2411). Should be ignored by the receiver.
 * the 2431 will not sent this between repeats
 * /----...----\____...____ 125_on + 125_off
 * \endverbatim
 */
constexpr unsigned FER_INIT_WIDTH_US = 50000;  ///< \brief duration of lead-in-bit (TX-only)
constexpr unsigned FER_INIT_NEDGE_US = 25000;  ///< \brief duration until negative edge of lead-in-bit (TX-only)

/**
 * \verbatim
 * PREAMBLE - provides clock signal
 * before sending any data we have 7 pre-bits
 * /--\__ (2_on + 2_off = 4) * 7
 * (200us * 4 * 7)
 * \endverbatim
 */
constexpr unsigned FER_PRE_WIDTH_US = 800;  ///< \brief duration of preamble-bit
constexpr unsigned FER_PRE_NEDGE_US = 400;  ///< \brief duration until negative edge of preamble-bit

/**
 * \verbatim
 * STOP BIT - separates words
 * 1 stop bit comes before each preamble and each data word
 * /--\__________________ (2_on + 16_off = 18) * 1
 * (200us * 18 * 1)
 * \endverbatim
 */
constexpr unsigned FER_STP_WIDTH_US = 3600; ///< \brief duration of stop-bit
constexpr unsigned FER_STP_NEDGE_US = 400; ///< \brief duration until negative edge of stop-bit

/**
 * \verbatim
 * LEAD OUT BIT - ends last word in transmission
 * 1 stop bit comes before each preamble and each data word
 * /--\__________________ (2_on + 16_off = 18) * 1
 * (200us * 18 * 1)
 * \endverbatim
 */
constexpr unsigned FER_LEAD_OUT_WIDTH_US = 3600 * 4; ///< \brief duration of stop-bit
constexpr unsigned FER_LEAD_OUT_NEDGE_US = 400; ///< \brief duration until negative edge of stop-bit

/**
 * \verbatim
 * DATA BIT - encodes 1 or 0
 * data bits are represented by two different positions of the negative edge (on-off)
 * 1=short-long  /--\____  (2_on, 4_off)
 * 0=long-short  /----\__  (4_on, 2_off)
 * 10 bits makes one data-word
 * (200us * 6 * 10)
 * \endverbatim
 */
constexpr unsigned FER_BIT_WIDTH_US = 1200; ///< \brief duration of word-bit
constexpr unsigned FER_BIT_SHORT_US = 400; ///< \brief short duration until negative edge of word-bit
constexpr unsigned FER_BIT_LONG_US = 800; ///< \brief long duration until negative edge of word-bit
constexpr unsigned FER_BIT_WIDTH_MIN_US = 1400; ///< \brief duration of word-bit
constexpr unsigned FER_BIT_WIDTH_MAX_US = 1000; ///< \brief duration of word-bit

constexpr unsigned FER_INIT_WIDTH_MIN_US = 80 * 200;

constexpr unsigned FER_STP_WIDTH_MIN_US = 15 * 200;
constexpr unsigned FER_STP_WIDTH_MAX_US = 30 * 200;
constexpr unsigned FER_STP_NEDGE_MIN_US = 1 * 200;
constexpr unsigned FER_STP_NEDGE_MAX_US = 6 * 200;

#define E1 1 // XXX:experimental: 0=normal, 1=timings for sun-sensors preamble
//central: n-edge: 440us period: 800us
//sun: n-edge: 940, period: 1700us
#if E1 == 1
constexpr unsigned FER_PRE_WIDTH_MIN_US = 3 * 200; // 600us
constexpr unsigned FER_PRE_WIDTH_MAX_US = 10 * 200; // 2000us
constexpr unsigned FER_PRE_NEDGE_MIN_US = 1 * 200; // 200us
constexpr unsigned FER_PRE_NEDGE_MAX_US = 6 * 200; // 1200us
#else
constexpr unsigned FER_PRE_WIDTH_MIN_US = 3 * 200; // 600us
constexpr unsigned FER_PRE_WIDTH_MAX_US = 6 * 200; // 1200us
constexpr unsigned FER_PRE_NEDGE_MIN_US = 1 * 200; // 200us
constexpr unsigned FER_PRE_NEDGE_MAX_US = 4 * 200; // 800us
#endif

constexpr unsigned FER_BIT_SAMP_POS_US = 3 * 200; ///< for receiving: look if negative edge lies before or after


// -----stop------- ---------pre--    ----stop------     --data-word---     ----stop------     --data-word---     ----stop------      ...  + -- last data-word---
// (200us * 18 * 1) + (200us * 4 * 7) + (200us * 18 * 1) + (200us * 6 * 10) + (200us * 18 * 1) + (200us * 6 * 10) + (200us * 18 * 1)  ...  + (200us * 6 * 10)
/// \brief us-duration of a single data word (10bits)
constexpr unsigned FER_DAT_WORD_WIDTH_US = (FER_STP_WIDTH_US + (1UL * FER_BIT_WIDTH_US * FER_CMD_BIT_CT));
/// \brief us-duration of a complete timer message
constexpr unsigned FER_PRG_FRAME_WIDTH_US = (1UL * FER_DAT_WORD_WIDTH_US * FER_PRG_BYTE_CT * FER_WORD_PER_BYTE_CT * FER_PRG_PACK_CT);

