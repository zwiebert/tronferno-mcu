/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#include <fernotron/auto/fau_next_event.hh>
#include "timer_data_fs.h"
#include "timer_data_kvs.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "utils_misc/int_types.h"
#include "main_loop/main_queue.hh"



////////////////////////////////// public ////////////////////////////////////////////////////////////////////
static inline void fer_au_timer_data_change_cb() {
  mainLoop_callFun([]() {
    next_event_te.fer_am_updateTimerEvent(time(NULL));
  });
}

#ifdef CONFIG_APP_USE_TIMER_DATA_KVS
#define save_timer_data_fs save_timer_data_kvs
#define erase_timer_data_fs erase_timer_data_kvs
#define read_timer_data_fs read_timer_data_kvs
#endif


bool fer_stor_timerData_save(Fer_TimerData *p, uint8_t g, uint8_t m) {
  bool result = save_timer_data_fs(p, g, m);
  fer_au_timer_data_change_cb();
  return result;
}

bool fer_stor_timerData_erase(uint8_t g, uint8_t m) {
  if (erase_timer_data_fs(g, m)) {
    fer_au_timer_data_change_cb();
    return true;
  }
  return false;
}

bool fer_stor_timerData_load(Fer_TimerData *p, uint8_t *g, uint8_t *m, bool wildcard) {
  bool result = read_timer_data_fs(p, g, m, wildcard);
  return result;
}
