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


static void start_new_moving_mm(int mvi, gm_bitmask_t mm, u32 rt, bool direction) {
  u8 g;

  moving[mvi] = (struct mv){};

  for (g = 1; g <= GRP_MAX; ++g) {
    moving[mvi].mask[g] = mm[g];
  }
  moving[mvi].direction_up = direction;
  SET_BIT(moving_mask, mvi);
  moving[mvi].start_time = rt;
}

static void start_new_sunDownMoving_mm(int mvi, gm_bitmask_t mm, u32 rt) {
  u8 g, m;

  moving[mvi] = (struct mv ) { };

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (gm_GetBit(mm, g, m)) {
        if (!ferPos_mSunReadyToMoveDown(g, m))
          gm_ClrBit(mm, g, m);
      }
    }
    moving[mvi].mask[g] = mm[g];
  }
  moving[mvi].direction_up = false;
  moving[mvi].sun_down = true;
  SET_BIT(moving_mask, mvi);
  moving[mvi].start_time = rt;
}



#ifdef UNUSED_STATIC_FUNCTION
static void stop_moving_mm(int mvi, gm_bitmask_t mm, u32 rt) {
  struct mv *mv = &moving[mvi];
  int m, g;
  bool remaining = false;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (!gm_GetBit(mm, g, m))
        continue;

      gm_ClrBit(moving[mvi].mask, g, m);
      ferPos_mSetPct(0, g, m, ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, rt - mv->start_time));  // update pct table now
    }
    if (mv->mask[g]) {
      remaining = true;
    }
  }
  if (!remaining) {
    CLR_BIT(moving_mask, mvi);
  }
}
#endif

static int add_to_existing_movement_mm(gm_bitmask_t mm, u32 rt, bool direction) {
  u8 mvi, g;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi) && direction == moving[mvi].direction_up && rt == moving[mvi].start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        moving[mvi].mask[g] |= mm[g];
      }
      return mvi;
    }
  }
  return -1;
}


static int add_to_existing_sunDownMovement_mm(gm_bitmask_t mm, u32 rt) {
  u8 mvi, g, m;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi) && moving[mvi].sun_down && rt == moving[mvi].start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        for (m = 1; m <= MBR_MAX; ++m) {
          if (gm_GetBit(mm, g, m)) {
            if (!ferPos_mSunReadyToMoveDown(g, m))
              gm_ClrBit(mm, g, m);
          }
        }
        moving[mvi].mask[g] |= mm[g];
      }
      return mvi;
    }
  }
  return -1;
}

static int add_to_new_movement_mm(gm_bitmask_t mm, u32 rt, bool direction) {
  u8 mvi;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    start_new_moving_mm(mvi, mm, rt, direction);
    return mvi;
  }
  return -1;
}

static int add_to_new_sunMovement_mm(gm_bitmask_t mm, u32 rt) {
  u8 mvi;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    start_new_sunDownMoving_mm(mvi, mm, rt);
    return mvi;
  }
  return -1;
}

// register moving related commands sent to a shutter to keep track of its changing position
int
ferPos_mmMove(gm_bitmask_t mm, fer_cmd cmd) {
  u32 rt = run_time_10(0);
  int mvi;
  u8 g, m;

  bool isMoving = false, direction = false, isStopping = false, isSun = false;

  switch (cmd) {
  case fer_cmd_STOP:
    isStopping = true;
    break;
  case fer_cmd_SunUP:
    isSun = true;
  case fer_cmd_UP:
    isMoving = true;
    direction = 1;
    break;
  case fer_cmd_SunDOWN:
    isSun = true;
  case fer_cmd_DOWN:
    isMoving = true;
    direction = 0;
    break;
  default:
    return -1;
    break;

  }

  if (isMoving) {
    for (g = 1; g <= GRP_MAX; ++g) {
      for (m = 1; m <= MBR_MAX; ++m) {
        if (gm_GetBit(mm, g, m)) {
          u8 currPct = ferPos_getPct(0, g, m);
          if ((direction && currPct == PCT_UP) || (!direction && currPct == PCT_DOWN))
            gm_ClrBit(mm, g, m); // cannot move beyond end point
        }
      }
    }
  }

  if (moving_mask && (isStopping || isMoving)) {
    for (mvi = 0; mvi < mv_SIZE; ++mvi) {
      struct mv *mv = &moving[mvi];

      if (!GET_BIT(moving_mask, mvi))
        continue;

      for (g = 1; g <= GRP_MAX; ++g) {
        if (!mv->mask[g] || !mm[g])
          continue;
        for (m = 1; m <= MBR_MAX; ++m) {
          if (!gm_GetBit(mv->mask, g, m))
            continue;
          if (!gm_GetBit(mm, g, m))
            continue;

          if (isMoving && mv->direction_up == direction) {
            gm_ClrBit(mm, g, m);// already moving in right direction (XXX: is writing to argument ok?)
            continue;
          }

          stop_moving(mvi, g, m, rt);
        }
      }
    }
  }

  if (isMoving) {
    if (isSun && !direction) {  // XXX: if sun-up check if we are in sun-down position
      if (add_to_existing_sunDownMovement_mm(mm, rt) < 0) {
        add_to_new_sunMovement_mm(mm, rt);
      }
    } else {
      if (add_to_existing_movement_mm(mm, rt, direction) < 0) {
        add_to_new_movement_mm(mm, rt, direction);
      }
    }

  }

  return 0;
}


int
ferPos_mMove(u32 a, u8 g, u8 m, fer_cmd cmd) {
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (pair_getControllerPairings(a, &gm))
      return ferPos_mmMove(gm, cmd);
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

  return ferPos_mmMove(mm, cmd);
}

