#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app/config/proj_app_cfg.h"

#include "app/settings/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "misc/time/run_time.h"
#include "app/uout/status_output.h"
#include "uout/status_json.hh"
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

static int fer_pos_filter_mm(Fer_GmBitMask *mm, struct filter *filter) {

  int remaining = 0;


    for (auto it = mm->begin(); it; ++it) {
      u8 g = it.getG(), m = it.getM();

    if (filter->sun_not_ready_to_move_down && !fer_pos_shouldMove_sunDown(g, m))
      goto filter_out;

    if (filter->destination_already_reached) {
      u8 currPct = fer_statPos_getPct(0, g, m);
      if ((direction_isUp(filter->dir) && currPct == PCT_UP) || (direction_isDown(filter->dir) && currPct == PCT_DOWN))
        goto filter_out;
    }

    ++remaining;
    continue;
    filter_out:
    mm->clearBit(g, m);

  }

  if (filter->same_direction) {
    struct Fer_Move *Fer_Move;
    for (Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {
      for (auto it = mm->begin(); it; ++it) {
        u8 g = it.getG(), m = it.getM();
        if (!Fer_Move->mask.getBit(g, m))
          continue;

        if (filter->same_direction) {
          if ((direction_isUp(Fer_Move->dir) && direction_isUp(filter->dir)) || (direction_isDown(Fer_Move->dir) && direction_isDown(filter->dir))) {
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


static void fer_pos_createMovement_mm(struct Fer_Move *Fer_Move, Fer_GmBitMask *mm, u32 now_ts, enum direction dir) {
  u8 g;

  for (g = 1; g <= GRP_MAX; ++g) {
    Fer_Move->mask[g] = mm->getByte(g);
  }

  Fer_Move->start_time = now_ts;
  Fer_Move->dir = dir;
}

struct Fer_Move *fer_pos_addToMovement_mm(Fer_GmBitMask *mm, u32 now_ts, enum direction dir) {
  u8 g;
  struct Fer_Move *Fer_Move;

  for (Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {
    if (dir == Fer_Move->dir && now_ts == Fer_Move->start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        Fer_Move->mask[g] |= mm->getByte(g);
      }
      return Fer_Move;
    }
  }
  return 0;
}

static struct Fer_Move* add_to_new_movement_mm(Fer_GmBitMask *mm, u32 now_ts, enum direction dir) {
  struct Fer_Move *Fer_Move = fer_mv_calloc();

  if (Fer_Move) {
      fer_pos_createMovement_mm(Fer_Move, mm, now_ts, dir);
  }
  return Fer_Move;
}

// register moving related commands sent to a shutter to keep track of its changing position
int fer_simPos_registerMovingShutters(Fer_GmBitMask *mm, fer_cmd cmd) {
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

  fer_pos_filter_mm(mm, &filter);

  if (mm->isAllClear())
    return -1;

  if (direction_isStop(dir) || direction_isMove(dir)) {
    fer_pos_stop_mm(mm, now_ts);
  }

  if (direction_isMove(dir)) {
    if (!fer_pos_addToMovement_mm(mm, now_ts, dir)) {
      add_to_new_movement_mm(mm, now_ts, dir);
    }
  }

  return 0;
}


int fer_simPos_registerMovingShutter(u32 a, u8 g, u8 m, fer_cmd cmd) {
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == cfg_getCuId())) {
    Fer_GmBitMask gm;
    if (fer_alias_getControllerPairings(a, &gm))
      return fer_simPos_registerMovingShutters(&gm, cmd);
    return 0;
  }
#endif

  Fer_GmBitMask mm ;
  if (g == 0) {
    for (g = 1; g <= GRP_MAX; ++g) {
      mm[g] = 0xfe;
    }
  } else if (m == 0) {
     mm[g] = 0xfe;
  } else {
    mm.setBit(g, m);
  }

  return fer_simPos_registerMovingShutters(&mm, cmd);
}

