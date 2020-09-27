/*
 * callbacks.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include "fernotron/fsb.h"  //XX
#include "fernotron/fer_msg_attachment.h" // XXX
#include "fernotron/trx/timer_data.h"

extern void (*fer_beforeFirstSend_cb)(const fer_sbT *fsb);
extern void (*fer_beforeAnySend_cb)(fer_msg_type msg_type, const fer_sbT *fsb, const fer_rawMsg *fmsg);
extern void (*fer_rawMessageReceived_cb)(fer_msg_type msg_type, const fer_sbT *fsb, const fer_rawMsg *fmsg);
extern void (*fer_plainMessageReceived_cb)(const fer_sbT *fsb);
extern void (*fer_rtcMessageReceived_cb)(fer_rawMsg *fmsg);
extern void (*fer_timerMessageReceived_cb)(fer_rawMsg *fmsg);

