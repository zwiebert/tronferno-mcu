/**
 * \file fernotron_trx/raw/fer_msg_tx.h
 * \brief  Send raw messages
 * \author bertw
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>

#include "fernotron_trx/raw/fer_fsb.h"
#include "fernotron_trx/fer_trx_c_api.h"

#ifdef __cplusplus
  extern "C" {
#endif

/**
 * \brief Set this to start RF transmission. It stays true until transmission is done.
 */
extern volatile bool fer_tx_messageToSend_isReady;

/**
 * \brief          Push a raw message to send queue
 * \param fsb      plain part of the message
 * \param msgType  Message kind.  If not a plain message, the extended message data must be already in buffer \link fer_rawMsg_buf_tx \endlink
 * \param repeats  Repeat sending the message N times
 * \param delay    To delay the actual sending of message (in s/10)
 * \return         true for success
 */
bool fer_send_msg(const fer_sbT *fsb, fer_msg_type msgType, int8_t repeats, uint16_t delay = 0);

/**
 * \brief          Push an RTC message to send queue
 * \param fsb      raw plain part of the message
 * \param rtc      RTC data to set as timestamp (will be converted just before transmitting)
 * \param repeats  Repeat sending the message N times
 * \return         true for success
 */
bool fer_send_msg_rtc(const fer_sbT *fsb, time_t rtc, i8 repeats);



/**
 * \brief           Push the plain message to send queue and also a delayed STOP message
 * \param fsb       Plain message
 * \param delay     To delay the actual sending of message (in s/10)
 * \param stopDelay Interval between (first) original message and STOP message (in s/10)
 * \param repeats  Repeat sending the (first) message N times
 * \return         true for success
 */
bool fer_send_msg_with_stop(const fer_sbT *fsb, uint16_t delay, uint16_t stopDelay, int8_t repeats);


/**
 * \brief                Increment a toggle nibble (will overflow at 0xf)
 * \param toggle_nibble  current value
 * \param step           step to increment
 * \return               incremented value
 */
uint8_t fer_tglNibble_ctUp(uint8_t toggle_nibble, int step);

/**
 * \brief               Get length of send queue
 * \return              number of message in the queue
 */
int fer_tx_get_msgPendingCount();

#ifdef __cplusplus
  }
#endif
