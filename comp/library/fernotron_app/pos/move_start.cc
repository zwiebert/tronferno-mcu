#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app/config/proj_app_cfg.h"

#include "app/settings/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "misc/time/run_time.h"
#include "app/uout/status_output.h"
#include "uout/status_json.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"

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




struct filter {
  bool same_direction : 1;
  bool sun_not_ready_to_move_down : 1;
  bool destination_already_reached : 1;
  enum direction dir;
};

static int ferPos_filter_mm(GmBitMask *mm, struct filter *filter) {

  int remaining = 0;


    for (auto it = mm->begin(); it; ++it) {
      u8 g = it.getG(), m = it.getM();

    if (filter->sun_not_ready_to_move_down && !ferPos_shouldMove_sunDown(g, m))
      goto filter_out;

    if (filter->destination_already_reached) {
      u8 currPct = statPos_getPct(0, g, m);
      if ((direction_isUp(filter->dir) && currPct == PCT_UP) || (direction_isDown(filter->dir) && currPct == PCT_DOWN))
        goto filter_out;
    }

    ++remaining;
    continue;
    filter_out:
    mm->clearBit(g, m);

  }

  if (filter->same_direction) {
    struct mv *mv;
    for (mv = mv_getFirst(); mv; mv = mv_getNext(mv)) {
      for (auto it = mm->begin(); it; ++it) {
        u8 g = it.getG(), m = it.getM();
        if (!mv->mask.getBit(g, m))
          continue;

        if (filter->same_direction) {
          if ((direction_isUp(mv->dir) && direction_isUp(filter->dir)) || (direction_isDown(mv->dir) && direction_isDown(filter->dir))) {
            goto filter_out_mv;
            continue;
          }
        }

        continue;
        filter_out_mv: --remaining;
        mm->clearBit(g, m);
      }
    }
  }
  return remaining;
}


static void ferPos_createMovement_mm(struct mv *mv, GmBitMask *mm, u32 now_ts, enum direction dir) {
  u8 g;

  for (g = 1; g <= GRP_MAX; ++g) {
    mv->mask[g] = mm->getByte(g);
  }

  mv->start_time = now_ts;
  mv->dir = dir;
}

struct mv *ferPos_addToMovement_mm(GmBitMask *mm, u32 now_ts, enum direction dir) {
  u8 g;
  struct mv *mv;

  for (mv = mv_getFirst(); mv; mv = mv_getNext(mv)) {
    if (dir == mv->dir && now_ts == mv->start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        mv->mask[g] |= mm->getByte(g);
      }
      return mv;
    }
  }
  return 0;
}

static struct mv* add_to_new_movement_mm(GmBitMask *mm, u32 now_ts, enum direction dir) {
  struct mv *mv = mv_calloc();

  if (mv) {
      ferPos_createMovement_mm(mv, mm, now_ts, dir);
  }
  return mv;
}

// register moving related commands sent to a shutter to keep track of its changing position
int simPos_registerMovingShutters(GmBitMask *mm, fer_cmd cmd) {
  u32 now_ts = get_now_time_ts();

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

  if (mm->isAllClear())
    return -1;

  if (direction_isStop(dir) || direction_isMove(dir)) {
    ferPos_stop_mm(mm, now_ts);
  }

  if (direction_isMove(dir)) {
    if (!ferPos_addToMovement_mm(mm, now_ts, dir)) {
      add_to_new_movement_mm(mm, now_ts, dir);
    }
  }

  return 0;
}


int simPos_registerMovingShutter(u32 a, u8 g, u8 m, fer_cmd cmd) {
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == cfg_getCuId())) {
    GmBitMask gm;
    if (pair_getControllerPairings(a, &gm))
      return simPos_registerMovingShutters(&gm, cmd);
    return 0;
  }
#endif

  GmBitMask mm ;
  if (g == 0) {
    for (g = 1; g <= GRP_MAX; ++g) {
      mm[g] = 0xfe;
    }
  } else if (m == 0) {
     mm[g] = 0xfe;
  } else {
    mm.setBit(g, m);
  }

  return simPos_registerMovingShutters(&mm, cmd);
}

