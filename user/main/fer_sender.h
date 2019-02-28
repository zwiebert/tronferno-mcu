/*
 * fer_sender.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_SENDER_H_
#define USER_MAIN_FER_SENDER_H_


extern volatile bool is_sendMsgPending;

void ftx_tick(void);
bool fer_send_prg(fer_sender_basic *fsb);
bool fer_send_cmd(fer_sender_basic *fsb);
bool fer_send_msg(fer_sender_basic *fsb, fmsg_type msgType);

bool fers_loop(void);
#endif /* USER_MAIN_FER_SENDER_H_ */
