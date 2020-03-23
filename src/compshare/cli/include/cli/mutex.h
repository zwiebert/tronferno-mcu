/*
 * mutex.h
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#ifndef SRC_ESP32_MAIN_MUTEX_H_
#define SRC_ESP32_MAIN_MUTEX_H_

#include "stdbool.h"

#ifdef USE_MUTEX
bool mutex_cliTake();
bool mutex_cliTakeTry();
void mutex_cliGive();
void mutex_setup();

#else
#define mutex_cliTake() true
#define mutex_cliTryTake() true
#define mutex_cliGive()
#define mutex_setup()
#endif

#endif /* SRC_ESP32_MAIN_MUTEX_H_ */
