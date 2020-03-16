/*
 * shutter_movement.c
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */
#include <string.h>

#include "fernotron_pos/shutter_pct.h"
#include "fernotron_pos/shutter_prefs.h"

#include "app/proj_app_cfg.h"

#include "config/config.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include "userio_app/status_output.h"
#include "userio/status_json.h"
#include "fernotron_alias/pairings.h"
#include "fernotron_auto/timer_data.h"
#include "cli_app/cli_imp.h"
#include "cli_app/cli_fer.h"
#include "cli/mutex.h"

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



struct mv moving[mv_SIZE];
u8 moving_mask;
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


static void ferPos_printMovingPct(u8 g, u8 m, u8 pct) {
  if (mutex_cliTake()) {
    so_arg_gmp_t gmp = { g, m, pct };
    if (sj_open_root_object("tfmcu")) {
      so_output_message(SO_POS_PRINT_GMP, &gmp);
      sj_close_root_object();
      cli_print_json(sj_get_json());
    }
    mutex_cliGive();
  }
}

// check if a moving shutter has reached its end position
int ferPos_mvCheck_mv(struct mv *mv, unsigned now_time_ts) {
  u8 g, m, pct;
  int stopped_count = 0;

  u16 duration_ts = now_time_ts - mv->start_time;

  for (g = 1; g < 8; ++g) {
    for (m = 1; m < 8; ++m) {
      if (!gm_GetBit(mv->mask, g, m))
        continue;

      pct = ferPos_calcMovePct_fromDirectionAndDuration_m(g, m, mv_dirUp(mv), duration_ts);

      if ((mv_dirUp(mv) && pct == PCT_UP) || (!mv_dirUp(mv) && pct == PCT_DOWN)) {
        ferPos_stop_mv(mv, g, m, pct);
        ++stopped_count;
        continue;
      }

      if (mv->dir == DIRECTION_SUN_DOWN && ferPos_mCalcSunStop(g, m, duration_ts)) {
        ferPos_stop_mv(mv, g, m, pct);
        ++stopped_count;
        continue;
      }

      ferPos_printMovingPct(g, m, pct);
    }
  }
  return stopped_count;
}


void ferPos_mvCheck_mvi(u8 mvi) {
  if (ferPos_mvCheck_mv(&moving[mvi], get_now_time_ts(0))) {
    ferPos_freeMvIfUnused_mvi(mvi);
  }
}


static void ferPos_mvCheck() {
  u8 i;
  u8 mask = moving_mask;

  for (i = 0; mask != 0; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;

    ferPos_mvCheck_mvi(i);
  }
}


void ferPos_mvCheck_iv(int interval_ts) {
  static int next_s10;
  if (moving_mask && next_s10 < get_now_time_ts()) {
    next_s10 = get_now_time_ts() + interval_ts;
    ferPos_mvCheck();
  }
}

bool ferPos_mCalcSunStop(u8 g, u8 m, u16 time_s10) {
  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);

  int pct = ferPos_getPct(0, g, m);

  int t = st.move_down_tsecs * (100 - pct) / 100;

  if (t + time_s10 >= st.move_sundown_tsecs)
    return true;

  return false;
}

bool ferPos_sunReadyToMoveDown_m(u8 g, u8 m) {
  u8 pct_curr = ferPos_getPct(0, g, m);
  u8 pct_sun = ferPos_mGetSunPct(g, m);

  if (pct_curr <= pct_sun)
    return false;

  if (gm_GetBit(manual_bits, g, m))
    return false;

  timer_data_t td = { };
  if (read_timer_data(&td, &g, &m, true)) {
    if (!td_is_sun_auto(&td))
      return false;
  }

  return true;
}




u16 ferPos_calcMoveDuration_fromPctDiff_m(u8 g, u8 m, u8 curr_pct, u8 pct) {
  bool direction = curr_pct < pct;
  u8 pct_diff = direction ? pct - curr_pct : curr_pct - pct;

  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);

  u16 result = (direction ? pct_diff * st.move_up_tsecs : pct_diff * st.move_down_tsecs) / 100;

  return result;
}



u8 ferPos_calcMovePct_fromDirectionAndDuration_m(u8 g, u8 m, bool direction_up, u16 time_s10) {
  int pct = ferPos_getPct(0, g, m);
  struct shutter_timings st;
  ferPos_mGetSt(&st, g, m);

  if (pct == -1) {
    return direction_up ? PCT_UP : PCT_DOWN;// XXX
  }

  int add = (direction_up ? 100 * time_s10 /  st.move_up_tsecs : -100 * time_s10 / st.move_down_tsecs);
  add += pct;

  if (add > 100)
    return 100;
  else if (add < 0)
    return 0;
  else
    return add;
}



int ferPos_mGetMovingPct(u32 a, u8 g, u8 m) {
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    return -1;
  }

  u32 rt = get_now_time_ts(0);
  u8 i;
  u8 mask = moving_mask;

  for (i = 0; mask != 0; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;

    struct mv *mv = &moving[i];

    if (gm_GetBit(mv->mask, g, m)) {
      u16 time_s10 = rt - mv->start_time;
      u8 pct = ferPos_calcMovePct_fromDirectionAndDuration_m(g, m, mv_dirUp(mv), time_s10);
      return pct;
    }
  }
  return ferPos_mGetPct(a, g, m);
}

#ifdef NO_OPT
#pragma GCC pop_options
#endif