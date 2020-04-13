/*
 * timer.h
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_APP_INCLUDE_APP_TIMER_H_
#define COMPONENTS_APP_INCLUDE_APP_TIMER_H_

#include <stdint.h>

void intTimer_setup(void);

#ifndef USE_ESP_GET_TIME
extern volatile uint32_t run_time_ts_, run_time_s_;
#endif

#endif /* COMPONENTS_APP_INCLUDE_APP_TIMER_H_ */
