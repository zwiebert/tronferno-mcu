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

extern volatile uint8_t frx_messageReceived;

extern volatile bool ftx_messageToSend_isReady;
extern volatile uint16_t ftx_messageToSend_wordCount;

// unlock read buffer after done with data
void frx_clear(void); // call it after received data buffers has been processed by main thread

// tick should be called from stable timer interrupt
// do a bit of work each tick
void frx_tick(void);  // call it from timer tick interrupt
void ftx_tick(void);  // call it from timer tick interrupt

typedef void (*frx_cb)(void); // callback must have IRAM_ATTR!
void frx_cbRegister_msgReceived(frx_cb cb);
//void frx_cbRegister_lostIpAddr(frx_cb cb);

#ifndef frx_MSG_RECEIVED_cb
#define frx_MSG_RECEIVED_cb() // do nothing
#endif

#ifndef ftx_MSG_TRANSMITTED_cb
#define ftx_MSG_TRANSMITTED_cb() // do nothing
#endif

#endif /* USER_MAIN_FER_TRANSCEIVER_H_ */
