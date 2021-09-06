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

#include "move.h"

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



struct shutter_timings st_def = { DEF_MV_UP_10, DEF_MV_DOWN_10, DEF_MV_SUN_DOWN_10 };


bool fer_pos_mGetSt(struct shutter_timings *st, u8 g, u8 m) {
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

u8 fer_pos_mGetSunPct(u8 g, u8 m) {
  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);
  int pct = 100 - st.move_sundown_tsecs * 100 / st.move_down_tsecs;
  return pct;
}

// check if a moving shutter has reached its end position
int fer_pos_mvCheck_mv(struct Fer_Move *Fer_Move, unsigned now_ts) {
  u8 pct;
  int stopped_count = 0;

  u16 duration_ts = now_ts - Fer_Move->start_time;

  for (Fer_Gm_Counter it; it; ++it) {
    const gT g = it.getG();
    const mT m = it.getM();
    if (!Fer_Move->mask.getMember(g, m))
      continue;

    pct = fer_simPos_getPct_afterDuration(g, m, direction_isUp(Fer_Move->dir), duration_ts);

    if ((direction_isUp(Fer_Move->dir) && pct == PCT_UP) || (!direction_isUp(Fer_Move->dir) && pct == PCT_DOWN)) {
      fer_pos_stop_mv(Fer_Move, g, m, pct);
      ++stopped_count;
      continue;
    }

    if (Fer_Move->dir == DIRECTION_SUN_DOWN && fer_pos_shouldStop_sunDown(g, m, duration_ts)) {
      fer_pos_stop_mv(Fer_Move, g, m, pct);
      ++stopped_count;
      continue;
    }

    uoApp_publish_pctChange_gmp(so_arg_gmp_t{g, m, pct});
  }

  return stopped_count;
}


void fer_pos_mvCheck_mvi(struct Fer_Move *Fer_Move) {
  if (fer_pos_mvCheck_mv(Fer_Move, get_now_time_ts()))
    if (Fer_Move->mask.isAllClear())
      fer_mv_free(Fer_Move);
}


void fer_pos_checkStatus_whileMoving() {
  struct Fer_Move *Fer_Move;
  for (Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {
    fer_pos_mvCheck_mvi(Fer_Move);
  }
}

void fer_pos_checkStatus_whileMoving_periodic(int interval_ts) {
  static unsigned next_ts;
  if (fer_mv_getFirst() && periodic_ts(interval_ts, &next_ts)) {
    fer_pos_checkStatus_whileMoving();
  }
}

bool fer_pos_shouldStop_sunDown(u8 g, u8 m, u16 duration_ts) {
  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);

  if (Pct pct = fer_statPos_getPct(g, m)) {
    int t = st.move_down_tsecs * (100 - pct.value()) / 100;
    if (t + duration_ts >= st.move_sundown_tsecs)
      return true;
  }

  return false;
}

bool fer_pos_shouldMove_sunDown(u8 g, u8 m) {

  if (fer_statPos_isSunPos(g, m))
    return false;

  u8 pct_curr = fer_statPos_getPct(g, m);
  u8 pct_sun = fer_pos_mGetSunPct(g, m);

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

bool fer_pos_shouldMove_sunUp(u8 g, u8 m) {
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

u16 fer_simPos_calcMoveDuration_fromPctDiff_m(u8 g, u8 m, u8 curr_pct, u8 pct) {
  bool direction = curr_pct < pct;
  u8 pct_diff = direction ? pct - curr_pct : curr_pct - pct;

  struct shutter_timings st;
  fer_pos_mGetSt(&st, g, m);

  u16 result = (direction ? pct_diff * st.move_up_tsecs : pct_diff * st.move_down_tsecs) / 100;

  return result;
}



Pct fer_simPos_getPct_afterDuration(u8 g, u8 m, bool direction_up, u16 duration_ts) {
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

Pct fer_simPos_getPct_whileMoving(u8 g, u8 m) {

  u32 now_ts = get_now_time_ts();

  struct Fer_Move *Fer_Move;
  for (Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {
    if (Fer_Move->mask.getMember(g, m)) {
      u16 duration_ts = now_ts - Fer_Move->start_time;
      u8 pct = fer_simPos_getPct_afterDuration(g, m, direction_isUp(Fer_Move->dir), duration_ts);
      return Pct(pct);
    }
  }
  return fer_statPos_getPct(g, m);
}

#ifdef NO_OPT
#pragma GCC pop_options
#endif
