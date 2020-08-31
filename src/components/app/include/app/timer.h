/*
 * timer.h
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>

void intTimer_setup(void);

#ifndef USE_ESP_GET_TIME
extern "C" volatile uint32_t run_time_ts_, run_time_s_;
#endif

