/*
 * mutex.h
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#ifndef SRC_ESP32_MAIN_MUTEX_H_
#define SRC_ESP32_MAIN_MUTEX_H_

#include "user_config.h"

#ifdef USE_MUTEX
bool mutex_cliTake();
void mutex_cliGive();
#else
#define mutex_cliTake() true
#define mutex_cliGive()
#endif

#endif /* SRC_ESP32_MAIN_MUTEX_H_ */
