/**
 * \file fernotron_trx/raw/fer_msg_tx.h
 * \brief  Send raw messages
 * \author bertw
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>

#include "fernotron_trx/raw/fer_fsb.h"
#include "app_config/proj_app_cfg.h"

#ifdef __cplusplus
  extern "C" {
#endif

/**
 * \brief Set this to start RF transmission. It stays true until transmission is done.
 */
extern volatile bool fer_tx_messageToSend_isReady;




/**
 * \brief          Will be called if the next delayed message is ready for transmission in TIME_TS
 *
 * \note           You can use this to start a timer interval which will call \link fer_tx_loop \endlink  after TIME_TS.
 *                 Until then there is no work to do. So calling fer_tx_loo would be wasteful.
 *
 * \param time_ts  Duration (in s/10) until the next message is ready for transmission
 */
extern void (*fer_tx_READY_TO_TRANSMIT_cb)(uint32_t time_ts);

// send short or long messages (data already in send-buffer)

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

/**
 * \brief   Do work.
 * \note    Call this from main thread in reaction to \link fer_tx_MSG_TRANSMITTED_ISR_cb  \endlink calls.
 * \note    Call this also from a timer interval in main thread, after pushing a delayed message (to avoid polling)
 * \note    Setup that interval in reaction to \link fer_tx_READY_TO_TRANSMIT_cb \endlink
 */
void fer_tx_loop(void);


#ifdef __cplusplus
  }
#endif
