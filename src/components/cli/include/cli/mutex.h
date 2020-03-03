/*
 * mutex.h
 *
 *  Created on: 13.07.2019
 *      Author: bertw
 */

#ifndef SRC_ESP32_MAIN_MUTEX_H_
#define SRC_ESP32_MAIN_MUTEX_H_

#include "stdbool.h"

bool mutex_cliTake();
void mutex_cliGive();

#endif /* SRC_ESP32_MAIN_MUTEX_H_ */
