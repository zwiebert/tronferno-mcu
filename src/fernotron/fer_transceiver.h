/*
 * fer_transceiver.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_TRANSCEIVER_H_
#define USER_MAIN_FER_TRANSCEIVER_H_

extern volatile bool has_cmdReceived;
extern volatile bool has_prgReceived;
extern volatile u8 MessageReceived;

// lock buffer for transmitting
bool recv_lockBuffer(bool enableLock);  // blocks receiver access to shared buffer when transmitting

// unlock read buffer after done with data
void frx_clear(void); // call it after received data buffers has been processed by main thread

// do a bit of work each tick
void frx_tick(void);  // call it from timer tick interrupt
void ftx_tick(void);  // call it from timer tick interrupt


#endif /* USER_MAIN_FER_TRANSCEIVER_H_ */
