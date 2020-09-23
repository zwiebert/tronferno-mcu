/*
 * timer_data.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#pragma once

#include "fernotron/timer_data.h"
#include "app/config/proj_app_cfg.h"


extern void (*fer_au_TIMER_DATA_CHANGE_cb)(void);

// save and read timer data on flash memory
// group  0...7
// member 0...7
// wildcard  if true, find the best matching data. may return timer g=1 m=0 data on a search for g=1 m=1
//
// when saving data with group=0 or menber=0, all matching old data will be deleted. So g=0 m=0 will delete all other data, because it addresses all receivers
bool fer_stor_timerData_save(Fer_TimerData *p, uint8_t group, uint8_t member);
bool fer_stor_timerData_load(Fer_TimerData *p, uint8_t *group, uint8_t *member, bool wildcard);
bool fer_stor_timerData_erase(u8 g, u8 m);


