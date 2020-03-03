/*
 * fer_rx_tx.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_FER_RX_TX_H_
#define COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_FER_RX_TX_H_

#include "fsb.h"
#include "fer_msg_basic.h"
#include "timer_data.h"
#include <time.h>

extern volatile bool is_sendMsgPending;
extern fsbT last_received_sender;

bool fer_tx_loop(void);
bool fer_rx_loop(void);

// send short or long messages (data already in send-buffer)
bool fer_send_msg(const fsbT *fsb, fmsg_type msgType, int8_t repeats);
bool fer_send_delayed_msg(const fsbT *fsb, fmsg_type msgType, uint16_t delay, int8_t repeats);
bool fer_send_msg_with_stop(const fsbT *fsb, uint16_t delay, uint16_t stopDelay, int8_t repeats);

bool send_rtc_message(fsbT *fsb, time_t rtc);
bool send_timer_message(fsbT *fsb, time_t rtc, timer_data_t *td);
bool send_empty_timer_message(fsbT *fsb, time_t rtc);


uint8_t fer_tglNibble_ctUp(uint8_t toggle_nibble, int step);
fsbT *get_fsb(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);


bool recv_lockBuffer(bool enableLock);  // blocks receiver access to shared buffer when transmitting



#endif /* COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_FER_RX_TX_H_ */