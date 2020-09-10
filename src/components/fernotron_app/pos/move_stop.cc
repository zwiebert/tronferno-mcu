/*
 * shutter_movement_stop.c
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */

#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app_config/proj_app_cfg.h"

#include "config/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "userio_app/status_output.h"
#include "userio/status_json.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "cli_app/cli_app.h"
#include "cli_app/cli_fer.h"
#include "cli/mutex.hh"

#include "move.h"

void ferPos_stop_mv(struct mv *mv, u8 g, u8 m, u8 pct) {
  statPos_setPct(0, g, m, pct);
  mv->mask.clearBit(g, m);
  if (mv->mask.isAllClear())
    mv_free(mv);
}

void ferPos_stop_mvi(struct mv *mv, u8 g, u8 m, u32 now_ts) {
  u8 pct = simPos_getPct_afterDuration(g, m, direction_isUp(mv->dir), now_ts - mv->start_time);
  ferPos_stop_mv(mv, g, m, pct);
}


void ferPos_stop_mm(gm_bitmask_t *mm, u32 now_ts) {

  struct mv *mv;
  for (mv = mv_getFirst(); mv; mv = mv_getNext(mv)) {

    for (auto it = mm->begin(); it; ++it) {
      gT g = it.getG(); mT m = it.getM();
      if (mv->mask.getBit(g, m))
        ferPos_stop_mvi(mv, g, m, now_ts);
    }
  }
}


void ferPos_stop_mvi_mm(struct mv *mv, gm_bitmask_t *mm, u32 now_ts) {
  u8 m, g;

  for (auto it = mm->begin(1); it; ++it) {
    u8 g = it.getG(), m = it.getM();
    mv->mask.clearBit(g, m);
    statPos_setPct(0, g, m, simPos_getPct_afterDuration(g, m, direction_isUp(mv->dir), now_ts - mv->start_time));  // update pct table now
  }

  if (mv->mask.isAllClear()) {
    mv_free(mv);
  }
}


