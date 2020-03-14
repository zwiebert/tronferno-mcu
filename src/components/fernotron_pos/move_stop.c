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

void ferPos_markMovingAsComplete(struct mv *mv, u8 g, u8 m, u8 pct) {
  ferPos_mSetPct(0, g, m, pct);
  gm_ClrBit(mv->mask, g, m);
}


void stop_moving(int mvi, u8 g, u8 m, u32 rt) {
  struct mv *mv = &moving[mvi];
  int gi;
  gm_ClrBit(moving[mvi].mask, g, m);
  ferPos_mSetPct(0, g, m, ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, rt - mv->start_time));  // update pct table now
  bool remaining = false;
  for (gi = 0; gi < 8; ++gi) {
    if (mv->mask[gi]) {
      remaining = true;
      break;
    }
  }
  if (!remaining) {
    CLR_BIT(moving_mask, mvi);
  }
}
