/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "user_config.h"
#include "timer_data.h"
#include "esp8266/data_flash2.h"
#include "automatic/timer_data_fs.h"

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
bool timer_data_changed;


bool  save_timer_data(DATA_TYPE *p, u8 g, u8 m) {
  timer_data_changed = true;
  return SAVE_TIMER_DATA_FUN(p, g, m);
}

bool  read_timer_data(DATA_TYPE *p, u8 *g, u8 *m, bool wildcard) {
  return READ_TIMER_DATA_FUN(p, g, m, wildcard);
}
