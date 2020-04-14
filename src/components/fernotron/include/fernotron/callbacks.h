/*
 * callbacks.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_CALLBACKS_H_
#define COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_CALLBACKS_H_

#include <stdint.h>
#include "fsb.h"
#include "fer_msg_attachment.h"
#include "timer_data.h"

extern void (*ferCb_beforeFirstSend)(const fsbT *fsb);
extern void (*ferCb_beforeAnySend)(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *fmsg);
extern void (*ferCb_rawMessageReceived)(fmsg_type msg_type, const fsbT *fsb, const fer_rawMsg *fmsg);
extern void (*ferCb_plainMessageReceived)(const fsbT *fsb);
extern void (*ferCb_rtcMessageReceived)(fer_rawMsg *fmsg);
extern void (*ferCb_timerMessageReceived)(fer_rawMsg *fmsg);

#endif /* COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_CALLBACKS_H_ */
