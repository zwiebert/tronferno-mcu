/*
 * shutter_movement.c
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */
#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app/config/proj_app_cfg.h"

#include "app/settings/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "app/uout/status_output.h"
#include "app/uout/callbacks.h"
#include "uout/cli_out.h"
#include "uout/status_json.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"
#include <fernotron/fer_main.h>
#include "misc/time/periodic.h"
#include "misc/time/run_time.h"

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


bool ferPos_mGetSt(struct shutter_timings *st, u8 g, u8 m) {
  *st = st_def;

  if (!ferPos_prefByM_load(st, g, m))
    return false;

  if (st->move_up_tsecs == 0)
    st->move_up_tsecs = st_def.move_up_tsecs;

  if (st->move_down_tsecs == 0)
    st->move_down_tsecs = st_def.move_down_tsecs;

  if (st->move_sundown_tsecs == 0)
    st->move_sundown_tsecs = st_def.move_sundown_tsecs;

  return true;
}

u8 ferPos_mGetSunPct(u8 g, u8 m) {
  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);
  int pct = 100 - st.move_sundown_tsecs * 100 / st.move_down_tsecs;
  return pct;
}

// check if a moving shutter has reached its end position
int ferPos_mvCheck_mv(struct mv *mv, unsigned now_ts) {
  u8 pct;
  int stopped_count = 0;

  u16 duration_ts = now_ts - mv->start_time;

  for (gm_iterator it; it; ++it) {
    const gT g = it.getG();
    const mT m = it.getM();
    if (!mv->mask.getBit(g, m))
      continue;

    pct = simPos_getPct_afterDuration(g, m, direction_isUp(mv->dir), duration_ts);

    if ((direction_isUp(mv->dir) && pct == PCT_UP) || (!direction_isUp(mv->dir) && pct == PCT_DOWN)) {
      ferPos_stop_mv(mv, g, m, pct);
      ++stopped_count;
      continue;
    }

    if (mv->dir == DIRECTION_SUN_DOWN && ferPos_shouldStop_sunDown(g, m, duration_ts)) {
      ferPos_stop_mv(mv, g, m, pct);
      ++stopped_count;
      continue;
    }

    uoApp_publish_pctChange_gmp(so_arg_gmp_t{g, m, pct});
  }

  return stopped_count;
}


void ferPos_mvCheck_mvi(struct mv *mv) {
  if (ferPos_mvCheck_mv(mv, get_now_time_ts()))
    if (mv->mask.isAllClear())
      mv_free(mv);
}


static void ferPos_checkStatus_whileMoving() {
  struct mv *mv;
  for (mv = mv_getFirst(); mv; mv = mv_getNext(mv)) {
    ferPos_mvCheck_mvi(mv);
  }
}

void ferPos_checkStatus_whileMoving_periodic(int interval_ts) {
  static unsigned next_ts;
  if (mv_getFirst() && periodic_ts(interval_ts, &next_ts)) {
    ferPos_checkStatus_whileMoving();
  }
}

bool ferPos_shouldStop_sunDown(u8 g, u8 m, u16 duration_ts) {
  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);

  int pct = statPos_getPct(0, g, m);

  int t = st.move_down_tsecs * (100 - pct) / 100;

  if (t + duration_ts >= st.move_sundown_tsecs)
    return true;

  return false;
}

bool ferPos_shouldMove_sunDown(u8 g, u8 m) {
  u8 pct_curr = statPos_getPct(0, g, m);
  u8 pct_sun = ferPos_mGetSunPct(g, m);

  if (pct_curr <= pct_sun)
    return false;

  if (manual_bits.getBit(g, m))
    return false;

  timer_data_t td = { };
  if (read_timer_data(&td, &g, &m, true)) {
    if (!td_is_sun_auto(&td))
      return false;
  }

  return true;
}

u16 simPos_calcMoveDuration_fromPctDiff_m(u8 g, u8 m, u8 curr_pct, u8 pct) {
  bool direction = curr_pct < pct;
  u8 pct_diff = direction ? pct - curr_pct : curr_pct - pct;

  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);

  u16 result = (direction ? pct_diff * st.move_up_tsecs : pct_diff * st.move_down_tsecs) / 100;

  return result;
}



u8 simPos_getPct_afterDuration(u8 g, u8 m, bool direction_up, u16 duration_ts) {
  int pct = statPos_getPct(0, g, m);
  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);

  if (pct == -1) {
    return direction_up ? PCT_UP : PCT_DOWN; // XXX
  }

  int add = (direction_up ? 100 * duration_ts / st.move_up_tsecs : -100 * duration_ts / st.move_down_tsecs);
  add += pct;

  if (add > 100)
    return 100;
  else if (add < 0)
    return 0;
  else
    return add;
}

int simPos_getPct_whileMoving(u32 a, u8 g, u8 m) {
  if (!(a == 0 || a == cfg_getCuId())) {
    return -1;
  }

  u32 now_ts = get_now_time_ts();

  struct mv *mv;
  for (mv = mv_getFirst(); mv; mv = mv_getNext(mv)) {
    if (mv->mask.getBit(g, m)) {
      u16 duration_ts = now_ts - mv->start_time;
      u8 pct = simPos_getPct_afterDuration(g, m, direction_isUp(mv->dir), duration_ts);
      return pct;
    }
  }
  return statPos_getPct(a, g, m);
}

void ferPos_loopCheckMoving() {
  ferPos_checkStatus_whileMoving_periodic(20);
}

#ifdef NO_OPT
#pragma GCC pop_options
#endif
