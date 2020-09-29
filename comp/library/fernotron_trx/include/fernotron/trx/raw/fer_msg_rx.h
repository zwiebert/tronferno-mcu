/*
 * fer_rx_tx.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include "fer_fsb.h"
#include "fer_msg_plain.h"
#include "fernotron/trx/timer_data.h"
#include <time.h>

extern fer_sbT last_received_sender;

#ifdef __cplusplus
  extern "C" {
#endif

void fer_rx_loop(void);

// send short or long messages (data already in send-buffer)
bool fer_send_msg(const fer_sbT *fsb, fer_msg_type msgType, int8_t repeats);
bool fer_send_delayed_msg(const fer_sbT *fsb, fer_msg_type msgType, uint16_t delay, int8_t repeats);
bool fer_send_msg_with_stop(const fer_sbT *fsb, uint16_t delay, uint16_t stopDelay, int8_t repeats);

bool fer_send_rtc_message(fer_sbT *fsb, time_t rtc);
bool fer_send_timer_message(fer_sbT *fsb, time_t rtc, const Fer_TimerData *td);
bool fer_send_empty_timer_message(fer_sbT *fsb, time_t rtc);

uint8_t fer_tglNibble_ctUp(uint8_t toggle_nibble, int step);
fer_sbT* fer_get_fsb(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);

// tick should be called from stable timer interrupt
// do a bit of work each tick
void fer_rx_tick(void);  // call it from timer tick interrupt
void fer_tx_dck(void);  // call it from timer tick interrupt

#ifdef __cplusplus
  }
#endif
