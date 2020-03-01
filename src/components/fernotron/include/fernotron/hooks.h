/*
 * hooks.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_HOOKS_H_
#define COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_HOOKS_H_

#include <stdint.h>
#include "fsb.h"
#include "fer_msg_extension.h"
#include "timer_data.h"

extern   uint8_t (*volatile ferHook_rx_pin)(void);
extern   void (*volatile ferHook_tx_pin)(uint8_t on);


extern void (*ferHook_beforeFirstSend)(const fsbT *fsb);
extern void (*ferHook_beforeAnySend)(fmsg_type msg_type, const fsbT *fsb, const fer_msg *fmsg);

extern void (*ferHook_rawMessageReceived)(fmsg_type msg_type, const fsbT *fsb, const fer_msg *fmsg);

extern void (*ferHook_plainMessageReceived)(const fsbT *fsb);
extern void (*ferHook_rtcMessageReceived)(fer_msg *fmsg);
extern void (*ferHook_timerMessageReceived)(timer_data_t *tdr);


// no hooks, but simple non standard functions implemented outside
void mcu_delay_us(uint16_t us);


#endif /* COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_HOOKS_H_ */
