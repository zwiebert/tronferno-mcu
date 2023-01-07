/**
 * \file     app_misc/timer.h
 * \brief    timer ISR to call transmit and receive functions for RF transceiver
 */

#pragma once

#include <stdint.h>

/// start timer ISR
void app_timerISR_setup(void);

#ifndef CONFIG_APP_USE_ESP_GET_TIME
volatile uint32_t run_time_ts_, run_time_s_;
#endif

