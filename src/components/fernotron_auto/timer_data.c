/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app/proj_app_cfg.h"
#include "timer_data.h"
#include "timer_data_fs.h"

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
bool timer_data_changed;


bool  save_timer_data(timer_data_t *p, u8 g, u8 m) {
  timer_data_changed = true;
  return SAVE_TIMER_DATA_FUN(p, g, m);
}

bool  read_timer_data(timer_data_t *p, u8 *g, u8 *m, bool wildcard) {
  return READ_TIMER_DATA_FUN(p, g, m, wildcard);
}
