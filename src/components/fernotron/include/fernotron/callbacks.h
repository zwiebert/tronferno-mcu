/*
 * callbacks.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include "fsb.h"
#include "fer_msg_attachment.h"
#include "timer_data.h"

extern void (*fer_beforeFirstSend_cb)(const fsbT *fsb);
extern void (*fer_beforeAnySend_cb)(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *fmsg);
extern void (*fer_rawMessageReceived_cb)(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *fmsg);
extern void (*fer_plainMessageReceived_cb)(const fsbT *fsb);
extern void (*fer_rtcMessageReceived_cb)(fer_rawMsg *fmsg);
extern void (*fer_timerMessageReceived_cb)(fer_rawMsg *fmsg);

