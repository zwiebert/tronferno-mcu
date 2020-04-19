/*
 * loop.h
 *
 *  Created on: 26.03.2020
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"

void loop_setBit_txLoop();
void loop_setBit_txLoop_fromISR();

void loop_setBit_rxLoop();
void loop_setBit_rxLoop_fromISR();

