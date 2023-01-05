/*
 * shutter_movement.c
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */
#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app_config/proj_app_cfg.h"

#include "fernotron/fer_main.h"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "app_uout/status_output.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "uout/cli_out.h"
#include "uout/status_json.hh"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"
#include <fernotron/fer_main.h>
#include "utils_time/periodic.h"
#include "utils_time/run_time.h"

#include "move.hh"

#ifdef DISTRIBUTION
#define D(x)
#else
#define D(x) x
//#define NO_OPT
#endif


#ifdef NO_OPT
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif


static bool fer_pos_shouldStop_sunDown(uint8_t g, uint8_t m, uint16_t duration_ts);

struct shutter_timings st_def = { DEF_MV_UP_10, DEF_MV_DOWN_10, DEF_MV_SUN_DOWN_10 };


bool fer_pos_mGetSt(struct shutter_timings *st, uint8_t g, uint8_t m) {
  *st = st_def;

  if (!fer_pos_prefByM_load(st, g, m))
    return false;

  if (st->move_up_tsecs == 0)
    st->move_up_tsecs = st_def.move_up_tsecs;

  if (st->move_down_tsecs == 0)
    st->move_down_tsecs = st_def.move_down_tsecs;

  if (st->move_sundown_tsecs == 0)
    st->move_sundown_tsecs = st_def.move_sundown_tsecs;

  return true;
}

uint8_t fer_pos_mGetSunPct(uint8_t g, uint8_t m) {
  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);
  int pct = 100 - st.move_sundown_tsecs * 100 / st.move_down_tsecs;
  return pct;
}

// check if a moving shutter has reached its end position
void fer_pos_checkStatus_whileMoving() {
  const uint32_t now_ts = get_now_time_ts();
  static unsigned last_call_ts;
  const bool publish = periodic_ts(10, &last_call_ts);

  so_arg_gmp_t pcts[50];
  int pcts_count = 0;

  for (struct Fer_Move *Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {
    uint16_t duration_ts = now_ts - Fer_Move->start_time;
    int stopped_count = 0;
    for (Fer_Gm_Counter it; it; ++it) {
      const gT g = it.getG();
      const mT m = it.getM();
      if (!Fer_Move->mask.getMember(g, m))
        continue;

      uint8_t pct = fer_simPos_getPct_afterDuration(g, m, direction_isUp(Fer_Move->dir), duration_ts);

      if (direction_isEndPos(Fer_Move->dir, pct) // end-position reached
      || (Fer_Move->dir == DIRECTION_SUN_DOWN && fer_pos_shouldStop_sunDown(g, m, duration_ts)) // sun-position reached
          ) {
        fer_pos_stop_mv(Fer_Move, g, m, pct);
        ++stopped_count;

      } else if (publish) {
        pcts[pcts_count++] = so_arg_gmp_t { .g = g, .m = m, .p = pct };
      }
    }

    if (stopped_count && Fer_Move->mask.isAllClear()) {
      fer_mv_free(Fer_Move); // freeing is OK here, because of how its implemented. fer_mv_getNext(Fer_Move) in loop head will still work fine
      continue;
    }
  }

  if (pcts_count) {
    uoApp_publish_pctChange_gmp(pcts, pcts_count);
  }
}

void fer_pos_checkStatus_whileMoving_periodic(int interval_ts) {
  static unsigned next_ts;
  if (fer_mv_getFirst() && periodic_ts(interval_ts, &next_ts)) {
    fer_pos_checkStatus_whileMoving();
  }
}

static bool fer_pos_shouldStop_sunDown(uint8_t g, uint8_t m, uint16_t duration_ts) {
  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);

  if (Pct pct = fer_statPos_getPct(g, m)) {
    int t = st.move_down_tsecs * (100 - pct.value()) / 100;
    if (t + duration_ts >= st.move_sundown_tsecs)
      return true;
  }

  return false;
}

bool fer_pos_shouldMove_sunDown(uint8_t g, uint8_t m) {

  if (fer_statPos_isSunPos(g, m))
    return false;

  uint8_t pct_curr = fer_statPos_getPct(g, m);
  uint8_t pct_sun = fer_pos_mGetSunPct(g, m);

  if (pct_curr <= pct_sun)
    return false;

  if (manual_bits.getMember(g, m))
    return false;

  Fer_TimerData td = { };
  if (fer_stor_timerData_load(&td, &g, &m, true)) {
    if (!td.getSunAuto())
      return false;
  }

  return true;
}

bool fer_pos_shouldMove_sunUp(uint8_t g, uint8_t m) {
  if (!fer_statPos_isSunPos(g, m))
    return false;

  if (manual_bits.getMember(g, m))
    return false;

  Fer_TimerData td = { };
  if (fer_stor_timerData_load(&td, &g, &m, true)) {
    if (!td.getSunAuto())
      return false;
  }

  return true;
}

uint16_t fer_simPos_calcMoveDuration_fromPctDiff_m(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct) {
  bool direction = curr_pct < pct;
  uint8_t pct_diff = direction ? pct - curr_pct : curr_pct - pct;

  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);

  uint16_t result = (direction ? pct_diff * st.move_up_tsecs : pct_diff * st.move_down_tsecs) / 100;

  return result;
}



Pct fer_simPos_getPct_afterDuration(uint8_t g, uint8_t m, bool direction_up, uint16_t duration_ts) {
  Pct pct = fer_statPos_getPct(g, m);
  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);

  if (!pct) {
    return direction_up ? PCT_UP : PCT_DOWN; // XXX
  }

  int add = (direction_up ? 100 * duration_ts / st.move_up_tsecs : -100 * duration_ts / st.move_down_tsecs);
  add += pct.value();

  if (add > 100)
    return 100;
  else if (add < 0)
    return 0;
  else
    return add;
}

Pct fer_simPos_getPct_whileMoving(uint8_t g, uint8_t m) {

  uint32_t now_ts = get_now_time_ts();

  struct Fer_Move *Fer_Move;
  for (Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {
    if (Fer_Move->mask.getMember(g, m)) {
      uint16_t duration_ts = now_ts - Fer_Move->start_time;
      uint8_t pct = fer_simPos_getPct_afterDuration(g, m, direction_isUp(Fer_Move->dir), duration_ts);
      return Pct(pct);
    }
  }
  return fer_statPos_getPct(g, m);
}

#ifdef NO_OPT
#pragma GCC pop_options
#endif
