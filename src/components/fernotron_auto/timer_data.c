/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "timer_data_fs.h"
#include "fernotron_auto/fau_tdata_store.h"
#include "misc/int_types.h"

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
bool timer_data_changed;


bool save_timer_data(timer_data_t *p, u8 g, u8 m) {
  bool result = save_timer_data_fs(p, g, m);
  timer_data_changed = true;
  fau_TIMER_DATA_CHANGE_cb();
  return result;
}

bool erase_timer_data(u8 g, u8 m) {
  if (erase_timer_data_fs(g, m)) {
    timer_data_changed = true;
    fau_TIMER_DATA_CHANGE_cb();
    return true;
  }
  return false;
}

bool read_timer_data(timer_data_t *p, u8 *g, u8 *m, bool wildcard) {
  bool result = read_timer_data_fs(p, g, m, wildcard);
  return result;
}
