/*
 * fer_sender.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_SENDER_H_
#define USER_MAIN_FER_SENDER_H_

#include "stdbool.h"
#include "fer_code.h"
#include "fer_frame.h"

// true while sending in progress
extern volatile bool is_sendMsgPending;

// send short or long messages (data already in send-buffer)
bool fer_send_msg(const fer_sender_basic *fsb, fmsg_type msgType);
bool fer_send_delayed_msg(const fer_sender_basic *fsb, fmsg_type msgType, u16 delay);
bool fer_send_msg_with_stop(const fer_sender_basic *fsb, u16 delay, u16 stopDelay);

// send repeats from this loop
bool fers_loop(void);

#endif /* USER_MAIN_FER_SENDER_H_ */
