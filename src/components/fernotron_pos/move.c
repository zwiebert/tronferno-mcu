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

struct mv moving[mv_SIZE];
u8 moving_mask;
struct shutter_timings st_def = { DEF_MV_UP_10, DEF_MV_DOWN_10, DEF_MV_SUN_DOWN_10 };


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
static void ferPos_mvCheck_mvi(u8 mvi) {
  u8 g, m, pct;

  struct mv *mv = &moving[mvi];
  u16 time_s10 = run_time_10(0) - mv->start_time;
  bool remaining = false;

  for (g = 1; g < 8; ++g) {
    for (m = 1; m < 8; ++m) {
      if (!gm_GetBit(mv->mask, g, m))
        continue;

      pct = ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, time_s10);

      if ((mv->direction_up && pct == PCT_UP) || (!mv->direction_up && pct == PCT_DOWN)) {
        ferPos_markMovingAsComplete(mv, g, m, pct);
        continue;
      }

      if (mv->sun_down && ferPos_mCalcSunStop(g, m, time_s10)) {
        ferPos_markMovingAsComplete(mv, g, m, pct);
        continue;
      }

      remaining = true;
      ferPos_printMovingPct(g, m, pct);
    }
  }

  if (!remaining)
    CLR_BIT(moving_mask, mvi);
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


void ferPos_checkMv_iv(int interval_ts) {
  static int next_s10;
  if (moving_mask && next_s10 < run_time_10()) {
    next_s10 = run_time_10() + interval_ts;
    ferPos_mvCheck();
  }
}


bool ferPos_mSunReadyToMoveDown(u8 g, u8 m) {

  if (get_state(0, g, m) != PCT_UP)
    return false;

  if (gm_GetBit(manual_bits, g, m))
    return false;

  if (ferPos_mCalcSunStop(g, m, 0))
    return false;

  timer_data_t td = {};
  if (read_timer_data(&td, &g, &m, true)) {
    if (!td_is_sun_auto(&td))
      return false;
  }

  return true;
}

bool ferPos_mCalcSunStop(u8 g, u8 m, u16 time_s10) {
  int pct = get_state(0, g, m);

  if (pct != 100) {
    return true; // XXX: sun-down must start at 100%
  }

  struct shutter_timings st = st_def;
  ferPos_prefByM_load(&st, g, m);

  if (st.move_sundown_tsecs == 0)
     st.move_sundown_tsecs = st_def.move_sundown_tsecs;

  return (time_s10 > st.move_sundown_tsecs);
}

u16 ferPos_mCalcMoveDuration_fromPctDiff(u8 g, u8 m, u8 curr_pct, u8 pct) {
  bool direction = curr_pct < pct;
  u8 pct_diff = direction ? pct - curr_pct : curr_pct - pct;
  struct shutter_timings st = st_def;
  ferPos_prefByM_load(&st, g, m);

  if (st.move_up_tsecs == 0)
     st.move_up_tsecs = st_def.move_up_tsecs;

  if (st.move_down_tsecs == 0)
     st.move_down_tsecs = st_def.move_down_tsecs;


  u16 result = (direction ? pct_diff * st.move_up_tsecs : pct_diff * st.move_down_tsecs) / 100;
#if 0
  if (curr_pct == 0 && direction)
    result += MV_UP_TO1;
  else if (pct == 0 && !direction)
    result += MV_DOWN_FROM1;
#endif
    return result;
}



u8 ferPos_mCalcMovePct_fromDirectionAndDuration(u8 g, u8 m, bool direction_up, u16 time_s10) {
  int pct = get_state(0, g, m);
  struct shutter_timings st = st_def;
  ferPos_prefByM_load(&st, g, m);

  if (st.move_up_tsecs == 0)
     st.move_up_tsecs = st_def.move_up_tsecs;

  if (st.move_down_tsecs == 0)
     st.move_down_tsecs = st_def.move_down_tsecs;

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

  u32 rt = run_time_10(0);
  u8 i;
  u8 mask = moving_mask;

  for (i = 0; mask != 0; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;

    struct mv *mv = &moving[i];

    if (gm_GetBit(mv->mask, g, m)) {
      u16 time_s10 = rt - mv->start_time;
      u8 pct = ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, time_s10);
      return pct;
    }
  }
  return ferPos_mGetPct(a, g, m);
}


