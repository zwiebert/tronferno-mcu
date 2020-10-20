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
void fer_msg_raw_checksumsCreate(fer_rawMsg *m, fer_msg_type t);

/**
 * \brief     Verify all checksums
 * \param m   Message pointer
 * \param t   Message kind
 * \return    true if checksums are valid
 */
bool fer_msg_raw_checksumsVerify(const fer_rawMsg *m, fer_msg_type t);

/**
 * \brief     Initialize an empty message (XXX: of the biggest type)
 * \param msg Message pointer to a buffer with MSG_TYPE_TIMER (XXX)
 */
void fer_msg_raw_init(fer_rawMsg *msg);

/**
 * \brief          Build the RTC part of message
 * \param msg      Message pointer to a buffer with MSG_TYPE_RTC or MSG_TYPE_TIMER
 * \param rtc      Current time to set the internal RTC of a device
 * \param rtc_only True if MSG_TYPE_RTC (XXX)
 */
void fer_msg_raw_from_rtc(fer_rawMsg *msg, time_t rtc, bool rtc_only);

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
