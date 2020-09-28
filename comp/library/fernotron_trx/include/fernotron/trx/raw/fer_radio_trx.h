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

#define WORDS_MSG_PLAIN (2*FER_BYTES_MSG_PLAIN)
#define WORDS_MSG_RTC (2*FER_BYTES_MSG_RTC)
#define WORDS_MSG_TIMER  (2*FER_BYTES_MSG_TIMER)


extern void (*fer_rx_MSG_RECEIVED_ISR_cb)(void);
extern void (*fer_tx_MSG_TRANSMITTED_ISR_cb)(void);


extern volatile fer_msg_type fer_rx_messageReceived;
extern volatile bool fer_tx_messageToSend_isReady;
extern volatile uint16_t fer_tx_messageToSend_wordCount;

#define fer_rx_hasNewMsg() (!!fer_rx_messageReceived)
#define fer_tx_isTransmitterBusy() (!!fer_tx_messageToSend_isReady)

#ifdef DISTRIBUTION
#define fer_rx_isReceiverBlocked() (fer_rx_messageReceived || fer_tx_messageToSend_isReady)
#else
#define fer_rx_isReceiverBlocked() (fer_rx_messageReceived)
#endif

// unlock read buffer after done with data
void fer_rx_clear(void); // call it after received data buffers has been processed by main thread
struct fer_rx_quality { uint8_t bad_pair_count; };
void fer_rx_getQuality(struct fer_rx_quality *dst);

// tick should be called from stable timer interrupt
// do a bit of work each tick
void fer_rx_tick(void);  // call it from timer tick interrupt
void fer_tx_dck(void);  // call it from timer tick interrupt

void fer_tx_transmitFerMsg(fer_rawMsg *msg, fer_msg_type msg_type);

void fer_rx_sampleInput(void); // call this from top of timer ISR handler
void fer_tx_setOutput(void); // call this from top of timer ISR handler



