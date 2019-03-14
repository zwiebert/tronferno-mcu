/*
 * fer_sender.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_SENDER_H_
#define USER_MAIN_FER_SENDER_H_

// true while sending in progress
extern volatile bool is_sendMsgPending;

// send short or long messages (data already in send-buffer)
bool fer_send_msg(fer_sender_basic *fsb, fmsg_type msgType);

// send repeats from this loop
bool fers_loop(void);

#endif /* USER_MAIN_FER_SENDER_H_ */
