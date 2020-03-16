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

#ifndef DISTRIBUTION
#define DB_INFO 0
#define DT(x)
#define D(x)
#else
#define DB_INFO 0
#define DT(x)
#define D(x)
#endif


static int gm_countSetBits(gm_bitmask_t mm) {
  u8 g, m;
  int remaining = 0;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (gm_GetBit(mm, g, m)) {
        ++remaining;
      }
    }
  }
  return remaining;
}

static bool gm_isEmpty(gm_bitmask_t mm) {
  u8 g;
  for (g = 1; g <= GRP_MAX; ++g)
    if (mm[g])
      return false;
  return true;
}

struct filter {
  bool same_direction : 1;
  bool sun_not_ready_to_move_down : 1;
  bool destination_already_reached : 1;
  enum direction dir;
};

static int ferPos_filter_mm(gm_bitmask_t mm, struct filter *filter) {
  u8 g, m, mvi;
  int remaining = 0;

  for (g = 1; g <= GRP_MAX; ++g) {
    if (!mm[g])
      continue;
    for (m = 1; m <= MBR_MAX; ++m) {
      if (gm_GetBit(mm, g, m)) {

        if (filter->sun_not_ready_to_move_down && !ferPos_sunReadyToMoveDown_m(g, m))
          goto filter_out;

        if (filter->destination_already_reached) {
          u8 currPct = ferPos_getPct(0, g, m);
          if ((direction_isUp(filter->dir) && currPct == PCT_UP) || (direction_isDown(filter->dir) && currPct == PCT_DOWN))
            goto filter_out;
        }

        ++remaining;
        continue;
        filter_out:
        gm_ClrBit(mm, g, m);
      }
    }
  }
#define filter_out filter_out2

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    struct mv *mv = &moving[mvi];
    if (!GET_BIT(moving_mask, mvi))
      continue;
    for (g = 1; g <= GRP_MAX; ++g) {
      if (!mv->mask[g] || !mm[g])
        continue;
      for (m = 1; m <= MBR_MAX; ++m) {
        if (gm_GetBit(mm, g, m)) {

          if (filter->same_direction) {
            if ((mv_dirUp(mv) && direction_isUp(filter->dir)) || (mv_dirDown(mv) && direction_isDown(filter->dir))) {
              goto filter_out;
              continue;
            }
          }

          ++remaining;
          continue;
          filter_out:
          gm_ClrBit(mm, g, m);
        }
      }
    }
  }
#undef filter_out
  return remaining;
}


static void ferPos_createMovement_mm(struct mv *mv, gm_bitmask_t mm, u32 now_time_ts, enum direction dir) {
  u8 g;

  for (g = 1; g <= GRP_MAX; ++g) {
    mv->mask[g] = mm[g];
  }

  mv->start_time = now_time_ts;
  mv->dir = dir;
}

static int ferPos_addToMovement_mm(gm_bitmask_t mm, u32 now_time_ts, enum direction dir) {
  u8 mvi, g;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi) && dir == moving[mvi].dir && now_time_ts == moving[mvi].start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        moving[mvi].mask[g] |= mm[g];
      }
      return mvi;
    }
  }
  return -1;
}

struct mv* ferPos_getFreeMv() {
  u8 mvi;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    SET_BIT(moving_mask, mvi);
    moving[mvi] = (struct mv){};
    return &moving[mvi];
  }
  return NULL;
}

static struct mv* add_to_new_movement_mm(gm_bitmask_t mm, u32 rt, enum direction dir) {
  struct mv *mv = ferPos_getFreeMv();

  if (mv) {
      ferPos_createMovement_mm(mv, mm, rt, dir);
  }
  return mv;
}

// register moving related commands sent to a shutter to keep track of its changing position
int ferPos_move_mm(gm_bitmask_t mm, fer_cmd cmd) {
  u32 rt = get_now_time_ts(0);
  int mvi;
  u8 g, m;

  enum direction dir = DIRECTION_NONE;
  struct filter filter = { };

  switch (cmd) {
  case fer_cmd_STOP:
    dir = DIRECTION_STOP;
    break;
  case fer_cmd_SunUP:
    dir = DIRECTION_SUN_UP;
    filter.destination_already_reached = true;
    break;
  case fer_cmd_UP:
    dir = DIRECTION_UP;
    filter.destination_already_reached = true;
    filter.same_direction = true;
    filter.same_direction = true;
    break;
  case fer_cmd_SunDOWN:
    dir = DIRECTION_SUN_DOWN;
    filter.destination_already_reached = true;
    filter.sun_not_ready_to_move_down = true;
    filter.same_direction = true; // XXX?
    break;
  case fer_cmd_DOWN:
    dir = DIRECTION_DOWN;
    filter.destination_already_reached = true;
    filter.same_direction = true;
    break;
  default:
    return -1;
    break;
  }

  ferPos_filter_mm(mm, &filter);

  if (gm_isEmpty(mm))
    return -1;

  if (direction_isStop(dir) || direction_isMove(dir)) {
    ferPos_stop_mm(mm, rt);
  }

  if (direction_isMove(dir)) {
    if (ferPos_addToMovement_mm(mm, rt, dir) < 0) {
      add_to_new_movement_mm(mm, rt, dir);
    }
  }

  return 0;
}


int ferPos_move_m(u32 a, u8 g, u8 m, fer_cmd cmd) {
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (pair_getControllerPairings(a, &gm))
      return ferPos_move_mm(gm, cmd);
    return 0;
  }
#endif

  gm_bitmask_t mm = { 0, };
  if (g == 0) {
    for (g = 1; g <= GRP_MAX; ++g) {
      mm[g] = 0xfe;
    }
  } else if (m == 0) {
    mm[g] = 0xfe;
  } else {
    gm_SetBit(mm, g, m);
  }

  return ferPos_move_mm(mm, cmd);
}

