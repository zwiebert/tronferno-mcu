/*
 * shutter_movement_stop.c
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

void ferPos_stop_mv(struct mv *mv, u8 g, u8 m, u8 pct) {
  ferPos_setPct(0, g, m, pct);
  gm_ClrBit(&mv->mask, g, m);
}

bool ferPos_freeMvIfUnused_mvi(int mvi) {
  int gi;
  for (gi = 0; gi < 8; ++gi) {
    if (moving[mvi].mask[gi]) {
      return false;
    }
  }
  CLR_BIT(moving_mask, mvi);
  return true;
}

void ferPos_stop_mvi(int mvi, u8 g, u8 m, u32 now_ts) {
  struct mv *mv = &moving[mvi];

  u8 pct = ferPos_getPct_afterDuration(g, m, mv_dirUp(mv), now_ts - mv->start_time);

  ferPos_stop_mv(mv, g, m, pct);
  ferPos_freeMvIfUnused_mvi(mvi);
}


void ferPos_stop_mm(gm_bitmask_t *mm, u32 now_ts) {
  u8 g, m, mvi;

  if (!moving_mask)
    return;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    struct mv *mv = &moving[mvi];

    if (!GET_BIT(moving_mask, mvi))
      continue;

    for (g = 1; g <= GRP_MAX; ++g) {
      if (!mv->mask[g] || !gm_GetByte(mm, g))
        continue;
      for (m = 1; m <= MBR_MAX; ++m) {
        if (!gm_GetBit(&mv->mask, g, m))
          continue;
        if (!gm_GetBit(mm, g, m))
          continue;

        ferPos_stop_mvi(mvi, g, m, now_ts);
      }
    }
  }
}



void ferPos_stop_mvi_mm(int mvi, gm_bitmask_t *mm, u32 now_ts) {
  struct mv *mv = &moving[mvi];
  int m, g;
  bool remaining = false;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (!gm_GetBit(mm, g, m))
        continue;

      gm_ClrBit(&moving[mvi].mask, g, m);
      ferPos_setPct(0, g, m, ferPos_getPct_afterDuration(g, m, mv_dirUp(mv), now_ts - mv->start_time));  // update pct table now
    }
    if (mv->mask[g]) {
      remaining = true;
    }
  }
  if (!remaining) {
    CLR_BIT(moving_mask, mvi);
  }
}

