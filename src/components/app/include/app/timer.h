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

extern volatile uint32_t run_time_s10;
#define get_now_time_ts(x) (run_time_s10 + 0)


#endif /* COMPONENTS_APP_INCLUDE_APP_TIMER_H_ */
