/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"
#include "timer_data_fs.h"
#include "timer_data_kvs.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "misc/int_types.h"

////////////////////////////////// public ////////////////////////////////////////////////////////////////////
void (*fer_au_TIMER_DATA_CHANGE_cb)(void);

static inline void fer_au_timer_data_change_cb() {
  if (fer_au_TIMER_DATA_CHANGE_cb)
    fer_au_TIMER_DATA_CHANGE_cb();
}

#ifdef USE_TIMER_DATA_KVS
#define save_timer_data_fs save_timer_data_kvs
#define erase_timer_data_fs erase_timer_data_kvs
#define read_timer_data_fs read_timer_data_kvs
#endif


bool fer_stor_timerData_save(Fer_TimerData *p, u8 g, u8 m) {
  bool result = save_timer_data_fs(p, g, m);
  fer_au_timer_data_change_cb();
  return result;
}

bool fer_stor_timerData_erase(u8 g, u8 m) {
  if (erase_timer_data_fs(g, m)) {
    fer_au_timer_data_change_cb();
    return true;
  }
  return false;
}

bool fer_stor_timerData_load(Fer_TimerData *p, u8 *g, u8 *m, bool wildcard) {
  bool result = read_timer_data_fs(p, g, m, wildcard);
  return result;
}
