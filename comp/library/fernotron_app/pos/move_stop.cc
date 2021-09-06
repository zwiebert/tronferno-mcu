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

#include "fernotron/fer_main.h"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "app_uout/status_output.h"
#include "uout/status_json.hh"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"

#include "move.h"
#include "move_buf.h"

void fer_pos_stop_mv(struct Fer_Move *Fer_Move, u8 g, u8 m, u8 pct) {
  fer_statPos_setPct(0, g, m, pct, direction_isSun(Fer_Move->dir) && direction_isDown(Fer_Move->dir));
  Fer_Move->mask.clearMember(g, m);
  if (Fer_Move->mask.isAllClear())
    fer_mv_free(Fer_Move);
}

void fer_pos_stop_mvi(struct Fer_Move *Fer_Move, u8 g, u8 m, u32 now_ts) {
  u8 pct = fer_simPos_getPct_afterDuration(g, m, direction_isUp(Fer_Move->dir), now_ts - Fer_Move->start_time);
  fer_pos_stop_mv(Fer_Move, g, m, pct);
}


void fer_pos_stop_mm(Fer_GmSet *mm, u32 now_ts) {

  struct Fer_Move *Fer_Move;
  for (Fer_Move = fer_mv_getFirst(); Fer_Move; Fer_Move = fer_mv_getNext(Fer_Move)) {

    for (auto it = mm->begin(); it; ++it) {
      gT g = it.getG(); mT m = it.getM();
      if (Fer_Move->mask.getMember(g, m))
        fer_pos_stop_mvi(Fer_Move, g, m, now_ts);
    }
  }
}


void fer_pos_stop_mvi_mm(struct Fer_Move *Fer_Move, Fer_GmSet *mm, u32 now_ts) {

  for (auto it = mm->begin(1); it; ++it) {
    u8 g = it.getG(), m = it.getM();
    Fer_Move->mask.clearMember(g, m);
    fer_statPos_setPct(0, g, m, fer_simPos_getPct_afterDuration(g, m, direction_isUp(Fer_Move->dir), now_ts - Fer_Move->start_time));  // update pct table now
  }

  if (Fer_Move->mask.isAllClear()) {
    fer_mv_free(Fer_Move);
  }
}



