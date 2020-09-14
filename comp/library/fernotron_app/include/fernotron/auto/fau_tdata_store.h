/*
 * timer_data.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#pragma once

#include "fernotron/timer_data.h"
#include "app/config/proj_app_cfg.h"


extern void (*fau_TIMER_DATA_CHANGE_cb)(void);

// save and read timer data on flash memory
// group  0...7
// member 0...7
// wildcard  if true, find the best matching data. may return timer g=1 m=0 data on a search for g=1 m=1
//
// when saving data with group=0 or menber=0, all matching old data will be deleted. So g=0 m=0 will delete all other data, because it addresses all receivers
bool save_timer_data(timer_data_t *p, uint8_t group, uint8_t member);
bool read_timer_data(timer_data_t *p, uint8_t *group, uint8_t *member, bool wildcard);
bool erase_timer_data(u8 g, u8 m);

extern bool timer_data_changed; // set by save_timer_data - cleared by code reading it

