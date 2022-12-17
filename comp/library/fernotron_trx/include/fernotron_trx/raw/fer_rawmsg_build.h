/**
 * \file   fernotron_trx/raw/fer_rawmsg_build.h
 * \brief  builder functions to construct (extended) raw messages
 */
#pragma once

#include <stdint.h>
#include "fer_fsb.h"
#include "fer_msg_attachment.h"
#include <time.h>

/**
 * \brief     Compute and fill in all checksums.
 * \note      Should be called after all parts of the message are build!
 * \param m   Message pointer
 * \param t   Message kind
 */
void fer_msg_raw_checksumsCreate(struct fer_raw_msg *m, fer_msg_type t);

/**
 * \brief     Verify all checksums
 * \param m   Message pointer
 * \param t   Message kind
 * \return    true if checksums are valid
 */
bool fer_msg_raw_checksumsVerify(const struct fer_raw_msg *m, fer_msg_type t);

/**
 * \brief     Initialize an empty message
 * \param msg Message pointer to a buffer with type T
 * \param t   Message type
 */
void fer_msg_raw_init(struct fer_raw_msg *msg, fer_msg_type t);


/**
 * \brief          Build the RTC part of message
 * \param msg      Message pointer to a buffer with MSG_TYPE_RTC or MSG_TYPE_TIMER
 * \param now      Current time to set the internal RTC of a device
 * \param rtc_only True if MSG_TYPE_RTC (XXX)
 */
void fer_msg_raw_from_time(fer_rawMsg *msg, time_t now, bool rtc_only);


/**
 * \brief          Set/Clear options in message
 * \param flags    \link fer_fpr00_FlagBitsValues  \endlink (e.g flag_Random, flag_SunAuto)
 * \param mask     Bit mask of the flags to set or clear
 */
void fer_msg_raw_from_flags(fer_rawMsg *msg, uint8_t flags, uint8_t mask);

/**
 * \brief         Build the weekly timer part of message
 */
void fer_msg_raw_from_weeklyTimer(fer_rawMsg *msg, const uint8_t *wtimer_data);

/**
 * \brief         Build the daily timer part of message
 */
void fer_msg_raw_from_dailyTimer(fer_rawMsg *msg, const uint8_t *dtimer_data);

/**
 * \brief         Build the astro timer part of message
 */
void fer_msg_raw_from_astro(fer_rawMsg *msg, int mint_offset);

/**
 * \brief         Build the footer part of message
 * \param msg     message buffer
 * \param a       device address (central unit)
 */
void fer_msg_raw_footerCreate(fer_rawMsg *msg, uint32_t a);







/**
 * \brief          Build the RTC part of message
 * \param msg      Message pointer to a buffer with MSG_TYPE_RTC or MSG_TYPE_TIMER
 * \param now      Current time to set the internal RTC of a device
 * \param rtc_only True if MSG_TYPE_RTC (XXX)
 */
void fer_msg_rtc_from_time(struct fer_rtc_sd *msg, time_t now, bool rtc_only);

/**
 * \brief          Set/Clear options in rtc line of message
 * \param flags    \link fer_fpr00_FlagBitsValues  \endlink (e.g flag_Random, flag_SunAuto)
 * \param mask     Bit mask of the flags to set or clear
 */
void fer_msg_from_flags(struct fer_rtc_sd *msg, uint8_t flags, uint8_t mask);
/**
 * \brief         Copy data to timer
 *                Note: this splits the data to leave room for the checksum bytes
 * \param msg     destination
 * \param data    source
 */
void fer_msg_from_weeklyTimer(union fer_wdtimer *msg, const uint8_t *wtimer_data);
/**
 * \brief         Copy data to timer
 *                Note: this splits the data to leave room for the checksum bytes
 * \param msg     destination
 * \param data    source
 */
void fer_msg_from_dailyTimer(union fer_wdtimer *msg, const uint8_t *dtimer_data);
/**
 * \brief         Copy data to timer
 *                Note: this splits the data to leave room for the checksum bytes
 * \param msg     destination
 * \param data    source
 */
void fer_msg_from_astro(union fer_astro *msg, int mint_offset);

