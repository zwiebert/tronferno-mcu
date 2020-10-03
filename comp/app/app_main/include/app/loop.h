/**
 * \file       app/loop.h
 * \brief      set flag bits which cause the related worker function to execute in main thread
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>

void loop_setBit_txLoop(uint32_t when_to_transmit_ts); ///< request to run TX worker function
void loop_setBit_txLoop_fromISR(); ///< request to run TX worker function (called from ISR)

void loop_setBit_rxLoop(); ///< request to run RX worker function
void loop_setBit_rxLoop_fromISR(); ///< request to run RX worker function (called from ISR)

void loop_setBit_pinNotifyInputChange_fromISR(); ///< request to run GPIO worker thread (called from ISR)

void loop_setBit_mcuRestart();  ///< request main thread to do a MCU restart
