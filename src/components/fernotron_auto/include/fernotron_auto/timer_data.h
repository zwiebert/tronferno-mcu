/*
 * timer_data.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_TIMER_DATA_H_
#define USER_MAIN_TIMER_DATA_H_

#include "fernotron/timer_data.h"

// save and read timer data on flash memory
// group  0...7
// member 0...7
// wildcard  if true, find the best matching data my return timer g=1 m=0 data on a search for g=1 m=1
//
// when saving data with group=0 or menber=0, all matching old data will be deleted. So g=0 m=0 will delete all other data, because it addresses all receivers
bool save_timer_data(timer_data_t *p, uint8_t group, uint8_t member);
bool read_timer_data(timer_data_t *p, uint8_t *group, uint8_t *member, bool wildcard);

extern bool timer_data_changed; // set by save_timer_data - cleared by code reading it



#endif /* USER_MAIN_TIMER_DATA_H_ */
