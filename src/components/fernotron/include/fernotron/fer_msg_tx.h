/*
 * fer_sender.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>

#include "fernotron/fsb.h"
#include "app/config/proj_app_cfg.h"

// true while sending in progress
extern volatile bool ftx_messageToSend_isReady;

extern void (*ftx_READY_TO_TRANSMIT_cb)(uint32_t time_ts);

// send short or long messages (data already in send-buffer)
bool fer_send_msg(const fsbT *fsb, fmsg_type msgType, int8_t repeats);
bool fer_send_delayed_msg(const fsbT *fsb, fmsg_type msgType, uint16_t delay_ts, int8_t repeats);
bool fer_send_msg_with_stop(const fsbT *fsb, uint16_t delay_ts, uint16_t stopDelay_ts, int8_t repeats);

// send repeats from this loop
void fer_tx_loop(void);


int ftx_get_msgPendingCount();



