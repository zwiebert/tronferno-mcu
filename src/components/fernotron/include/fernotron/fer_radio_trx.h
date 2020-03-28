/*
 * fer_fer_radio_trx.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_TRANSCEIVER_H_
#define USER_MAIN_FER_TRANSCEIVER_H_

#include <stdint.h>
#include "stdbool.h"
#include "app_config/callbacks.h"

extern volatile uint8_t frx_messageReceived;

extern volatile bool ftx_messageToSend_isReady;
extern volatile uint16_t ftx_messageToSend_wordCount;

// unlock read buffer after done with data
void frx_clear(void); // call it after received data buffers has been processed by main thread

// tick should be called from stable timer interrupt
// do a bit of work each tick
void frx_tick(void);  // call it from timer tick interrupt
void ftx_tick(void);  // call it from timer tick interrupt



// event notification callback functions

// called from ISR when frx_messageReceived is set
#ifndef frx_MSG_RECEIVED_ISR_cb
#define frx_MSG_RECEIVED_ISR_cb() // do nothing
#endif

// called from ISR when ftx_messageToSend_isReady is set
#ifndef ftx_MSG_TRANSMITTED_ISR_cb
#define ftx_MSG_TRANSMITTED_ISR_cb() // do nothing
#endif

#endif /* USER_MAIN_FER_TRANSCEIVER_H_ */
