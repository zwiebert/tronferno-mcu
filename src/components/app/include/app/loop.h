/*
 * loop.h
 *
 *  Created on: 26.03.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_APP_INCLUDE_APP_LOOP_H_
#define COMPONENTS_APP_INCLUDE_APP_LOOP_H_

#include "stdbool.h"

void loop_setBit_txLoop();
void loop_setBit_txLoop_fromISR();

void loop_setBit_rxLoop();
void loop_setBit_rxLoop_fromISR();

#endif /* COMPONENTS_APP_INCLUDE_APP_LOOP_H_ */
