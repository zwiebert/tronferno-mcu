/*
 * fer_sender.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_SENDER_H_
#define USER_MAIN_FER_SENDER_H_

#include "stdbool.h"
#include <stdint.h>

#include "fernotron/fsb.h"

// true while sending in progress
extern volatile bool ftx_messageToSend_isReady;

// send short or long messages (data already in send-buffer)
bool fer_send_msg(const fsbT *fsb, fmsg_type msgType, int8_t repeats);
bool fer_send_delayed_msg(const fsbT *fsb, fmsg_type msgType, uint16_t delay, int8_t repeats);
bool fer_send_msg_with_stop(const fsbT *fsb, uint16_t delay, uint16_t stopDelay, int8_t repeats);

// send repeats from this loop
bool fer_tx_loop(void);

#endif /* USER_MAIN_FER_SENDER_H_ */

