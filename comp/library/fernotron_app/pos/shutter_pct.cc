#include "app_config/proj_app_cfg.h"
#include "pos_map.hh"
#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "fernotron/fer_main.h"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "app_uout/status_output.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "uout/status_json.hh"
#include "uout/cli_out.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "utils_time/run_time.h"
#include "utils_time/periodic.h"
#include "main_loop/main_queue.hh"

#include "move.hh"

#ifndef DISTRIBUTION
#define DB_INFO 0
#define DT(x)
#define D(x) x
#else
#define DB_INFO 0
#define DT(x)
#define D(x)
#endif


static inline bool cfg_isMemberUnused(gT g, mT m) { return !fer_usedMemberMask.getMember(g, m); };

static char *g_to_name(uint8_t g, char *buf);


static Fer_Pos_Map pos_map;

void fer_pos_POSITIONS_SAVE_cb(bool has_unsaved) {
  static void *tmr;
  if (has_unsaved && !tmr) {
    tmr = mainLoop_callFunByTimer([]() {pos_map.autoSavePositions();}, 10000, true);
  }
  if (!has_unsaved && tmr) {
    mainLoop_stopFun(tmr);
    tmr = nullptr;
  }
}


Pct fer_statPos_getPct(uint8_t g, uint8_t m) {
  precond(g <= 7 && m <= 7);

  if (g == 0) {
    return Pct{}; // TODO: average all?
  } else {
    if (pos_map.isMemberUnused(g, m))
      return Pct{};
    return Pct(pos_map.getPct(g, m));
  }
  return Pct{};
}

bool fer_statPos_isSunPos(uint8_t g, uint8_t m) {
  precond(g <= 7 && m <= 7);

  if (g == 0 || m == 0) {
    return false;
  }
   return pos_map.isSunPos(g, m);
}
static void set_state(uint32_t a, uint8_t g, uint8_t m, int position, bool isSunPos) {
  DT(db_printf("%s: a=%x, g=%d, m=%d, position=%d\n", __func__, a, g, m, position));
  precond(g <= 7 && m <= 7);
  precond(0 <= position && position <= 100);

  pos_map.setMemberUnused(g,0);
  pos_map.setPct(g,m,position, isSunPos);
  pos_map.setPct(g,0, pos_map.getAvgPctGroup(g));

  fer_pos_POSITIONS_SAVE_cb(true);
}

void
fer_statPos_setPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct, bool isSunPos) {
  precond(g <= 7 && m <= 7);

#ifndef TEST_HOST
  DT(ets_printf("%s: a=%lx, g=%d, m=%d, pct=%d\n", __func__, a, (int)g, (int)m, (int)pct));
#ifdef CONFIG_APP_USE_PAIRINGS
  if (!(a == 0 || a == fer_config.cu)) {
    Fer_GmSet gm;
    if (fer_alias_getControllerPairings(a, &gm))
      for (Fer_Gm_Counter it(1,1,true); it; ++it) {
        const gT g = it.getG();
        const mT m = it.getM();
        if (gm.getMember(g, m)) {
          // recursion for each paired g/m
          fer_statPos_setPct(0, g, m, pct);
        }
    }
    return;
  }
#endif
#endif

  set_state(a, g, m, pct, isSunPos);


#ifndef TEST_HOST
  if (pct <= 100) {
    if (a == 0 || a == fer_config.cu) {

      so_arg_gmp_t gmp[2] = { { g, m, pct }, { g, 0, (uint8_t) pos_map.getPct(g, 0) } };
      uoApp_publish_pctChange_gmp(gmp, 2);
    }
  }
#endif
}

void fer_statPos_setPcts(Fer_GmSet *mm, uint8_t p) {
  for (Fer_Gm_Counter it(1, 1, true); it; ++it) {
    const gT g = it.getG();
    const mT m = it.getM();
    if (mm->getMember(g, m)) {
      fer_statPos_setPct(0, g, m, p);
    }
  }
}



void fer_statPos_printAllPcts(const struct TargetDesc &td) {
  soMsg_pos_begin(td);
  if (td.tgt() | SO_TGT_FLAG_TXT) {
    pos_map.fer_statPos_forEachPct([&td](const Fer_GmSet &same_pct, uint8_t pct) {
      soMsg_pos_print_mmp(td, { &same_pct, pct });
    });
  } else {
    const Fer_GmSet all = pos_map.getValidPcts();
    for (auto ia = all.begin(1); ia; ++ia) {
      const gT g = ia.getG();
      const mT m = ia.getM();
       soMsg_pos_print_gmp(td, {g, m, pos_map.getPct(g, m)});
    };
  }
  soMsg_pos_end(td);
}

void fer_pos_loop() {
  fer_pos_checkStatus_whileMoving_periodic(20);
  pos_map.autoSavePositions_iv(100);
}

void fer_pos_init() {
  for (uint8_t g = 0; g < 8; ++g) {
    pos_map.load_group_positions(g);
  }
}




