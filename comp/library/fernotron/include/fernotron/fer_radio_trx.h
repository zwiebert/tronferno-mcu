/*
 * fer_fer_radio_trx.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include "stdbool.h"
#include "app/config/proj_app_cfg.h"
#include "fer_msg_attachment.h"

#define WORDS_MSG_PLAIN (2*BYTES_MSG_PLAIN)
#define WORDS_MSG_RTC (2*BYTES_MSG_RTC)
#define WORDS_MSG_TIMER  (2*BYTES_MSG_TIMER)


extern void (*frx_MSG_RECEIVED_ISR_cb)(void);
extern void (*ftx_MSG_TRANSMITTED_ISR_cb)(void);


extern volatile fmsg_type frx_messageReceived;
extern volatile bool ftx_messageToSend_isReady;
extern volatile uint16_t ftx_messageToSend_wordCount;

#define frx_hasNewMsg() (!!frx_messageReceived)
#define ftx_isTransmitterBusy() (!!ftx_messageToSend_isReady)

#ifdef DISTRIBUTION
#define frx_isReceiverBlocked() (frx_messageReceived || ftx_messageToSend_isReady)
#else
#define frx_isReceiverBlocked() (frx_messageReceived)
#endif

// unlock read buffer after done with data
void frx_clear(void); // call it after received data buffers has been processed by main thread
struct frx_quality { uint8_t bad_pair_count; };
void frx_getQuality(struct frx_quality *dst);

// tick should be called from stable timer interrupt
// do a bit of work each tick
void frx_tick(void);  // call it from timer tick interrupt
void ftx_dck(void);  // call it from timer tick interrupt

void ftx_transmitFerMsg(fer_rawMsg *msg, fmsg_type msg_type);

void frx_sampleInput(void); // call this from top of timer ISR handler
void ftx_setOutput(void); // call this from top of timer ISR handler



