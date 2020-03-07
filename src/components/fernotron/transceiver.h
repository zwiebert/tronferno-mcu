/*
 * fer_transceiver.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_TRANSCEIVER_H_
#define USER_MAIN_FER_TRANSCEIVER_H_

#include <stdint.h>
#include "stdbool.h"


extern volatile bool has_cmdReceived;
extern volatile bool has_prgReceived;
extern volatile uint8_t MessageReceived;

// lock buffer for transmitting
bool recv_lockBuffer(bool enableLock);  // blocks receiver access to shared buffer when transmitting

// unlock read buffer after done with data
void frx_clear(void); // call it after received data buffers has been processed by main thread

#endif /* USER_MAIN_FER_TRANSCEIVER_H_ */
