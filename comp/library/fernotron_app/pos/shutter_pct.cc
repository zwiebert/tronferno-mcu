#include "app/config/proj_app_cfg.h"

#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app/settings/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "app/uout/status_output.h"
#include "app/uout/callbacks.h"
#include "uout/status_json.hh"
#include "uout/cli_out.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "misc/time/run_time.h"
#include "misc/time/periodic.h"

#include "move.h"

#ifndef DISTRIBUTION
#define DB_INFO 0
#define DT(x)
#define D(x) x
#else
#define DB_INFO 0
#define DT(x)
#define D(x)
#endif

#define GRP_COUNT 7
#define MBR_COUNT 7
#define GRP_MAX 7
#define MBR_MAX 7


#define cfg_isMemberUnused(g,m) !C.fer_usedMemberMask.getBit(g, m)


void (*fer_pos_POSITIONS_SAVE_cb)(bool has_unsaved);
static inline void fer_pos_POSITIONS_UNSAVED_cb() {
  if (fer_pos_POSITIONS_SAVE_cb)
    fer_pos_POSITIONS_SAVE_cb(true);
}
static inline void fer_pos_POSTIONS_SAVED_cb() {
  if (fer_pos_POSITIONS_SAVE_cb)
    fer_pos_POSITIONS_SAVE_cb(false);
}


static fer_shutterGroupPositionsT pos_map[8];
static u8  pos_map_changed;

enum { pm_GROUP_UNUSED=101, pm_MEMBER_UNUSED, pm_INVALID };
#define pm_getPct(g,m) (pos_map[(g)][(m)]+0)
#define pm_setPct(g,m,pct) pos_map[(g)][(m)] = (pct)
#define pm_setMemberUnused(g,m) pm_setPct((g),(m),pm_MEMBER_UNUSED)
#define pm_isMemberUnused(g, m) (pm_getPct((g),(m)) == pm_MEMBER_UNUSED)
#define pm_setGroupUnused(g) pm_setPct((g),0,pm_GROUP_UNUSED)
#define pm_isGroupUnused(g) (pm_getPct((g),0) == pm_GROUP_UNUSED)


static int fer_statPos_getAvgPctGroup(u8 g) {
  u8 m;
  int count=0, sum=0;
  for (m = 1; m <= MBR_MAX; ++m) {
    if (pm_isMemberUnused(g, m))
      continue;
    if (cfg_isMemberUnused(g,m))
      continue;
    ++count;
    sum += pm_getPct(g, m);
  }
  if (count) {
    return sum/count;
  }
  return pm_GROUP_UNUSED;
}

int
fer_statPos_getPct(u32 a, u8 g, u8 m) {
  precond(g <= 7 && m <= 7);

  if (g == 0) {
    return -1; // TODO: average all?
  } else {
    if (pm_isMemberUnused(g, m))
      return -1;
    return pm_getPct(g, m);
  }
  return -1;
}

static int 
set_state(u32 a, int g, int m, int position) {
  DT(db_printf("%s: a=%x, g=%d, m=%d, position=%d\n", __func__, a, g, m, position));
  precond(0 <= g && g <= 7 && 0 <= m && m <= 7);
  precond(0 <= position && position <= 100);

  pm_setMemberUnused(g,0);
  pm_setPct(g,m,position);
  pm_setPct(g,0,fer_statPos_getAvgPctGroup(g));

  SET_BIT(pos_map_changed, g);
  fer_pos_POSITIONS_UNSAVED_cb();
  return 0;
}

int 
fer_statPos_setPct(u32 a, u8 g, u8 m, u8 pct) {
  precond(0 < g && g <= 7 && 0 < m && m <= 7);
  int result = -1;


#ifndef TEST_HOST
  DT(ets_printf("%s: a=%lx, g=%d, m=%d, pct=%d\n", __func__, a, (int)g, (int)m, (int)pct));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == cfg_getCuId())) {
    Fer_GmBitMask gm;
    if (fer_alias_getControllerPairings(a, &gm))
      for (Fer_GmIterator it(1,1,true); it; ++it) {
        const gT g = it.getG();
        const mT m = it.getM();
        if (gm.getBit(g, m)) {
          // recursion for each paired g/m
          fer_statPos_setPct(0, g, m, pct);
        }
    }
    return 0;
  }
#endif
#endif

  result = set_state(a, g, m, pct);


#ifndef TEST_HOST
  if (pct <= 100) {
    if (a == 0 || a == cfg_getCuId()) {

      so_arg_gmp_t gmp[3] = { { g, m, pct }, { g, 0, (u8) pm_getPct(g, 0) }, { 0xff, 0xff, 0xff } };
      uoApp_publish_pctChange_gmp(gmp, 2);
    }
  }
#endif

  return result;
}

int fer_statPos_setPcts(Fer_GmBitMask *mm, u8 p) {
  for (Fer_GmIterator it(1, 1, true); it; ++it) {
    const gT g = it.getG();
    const mT m = it.getM();
    if (mm->getBit(g, m)) {
      fer_statPos_setPct(0, g, m, p);
    }
  }
  return 0;
}

int 
fer_statPos_printAllPcts(const struct TargetDesc &td) {
  Fer_GmBitMask msk;

  soMsg_pos_begin(td);
  for (gT g=1; g < 8; ++g) {
    if (pm_isGroupUnused(g))
      continue;
    for (mT m=0; m < 8; ++m) {
      if (pm_isMemberUnused(g,m))
        continue; //
      if (m != 0 && cfg_isMemberUnused(g,m))
        continue; //

      if (msk.getBit(g, m))
        continue; // was already processed by a previous pass

      u8 pct = pm_getPct(g,m);
      Fer_GmBitMask pos_msk;
      for (Fer_GmIterator it(g); it; ++it) {
        const gT g2 = it.getG();
        const mT m2 = it.getM();

        if (pm_getPct(g2,m2) == pct) {
          if (m2 != 0 && cfg_isMemberUnused(g2, m2))
            continue;
          pos_msk.setBit(g2, m2); // mark as being equal to pct
          msk.setBit(g2, m2); // mark as already processed
        }
      }
      so_arg_mmp_t mmp = { &pos_msk, pct };
      soMsg_pos_print_mmp(td, mmp);
    }
  }
  soMsg_pos_end(td);
  return 0;
}

static void fer_pos_autoSavePositions_iv(int interval_ts) {
  DT(ets_printf("%s: interval_tx=%d\n", __func__, interval_ts));
  static unsigned next_save_pos;
  u8 mask = pos_map_changed;
  if (mask && periodic_ts(interval_ts, &next_save_pos)) {
    fer_pos_POSTIONS_SAVED_cb();
    next_save_pos = 0;
    pos_map_changed = 0;

    u8 g;
    for (g = 0; mask; ++g, (mask >>= 1)) {
      if ((mask & 1) == 0)
        continue;

      fer_statPos_pctsByGroup_store(g, pos_map[g]);
      D(io_printf_v(vrbDebug, "autosave_pos: g=%d\n", (int)g));
    }
  }
}

void fer_pos_loop() {
  fer_pos_checkStatus_whileMoving_periodic(20);
  fer_pos_autoSavePositions_iv(100);
}

void fer_statPos_loopAutoSave() {
  fer_pos_autoSavePositions_iv(100);
}

void fer_pos_init() {
  u8 g, m;

  for (g=0; g < 8; ++g) {
    fer_statPos_pctsByGroup_load(g, pos_map[g]);
    if (pm_getPct(g,0) >= pm_INVALID)
      pm_setGroupUnused(g);
    for (m=1; m < 8; ++m) {
      if (pm_getPct(g,m) >= pm_INVALID)
        pm_setMemberUnused(g,m);
    }

  }
}

typedef u8 u8a8[8];


static char *g_to_name(u8 g, char *buf) {
  STRCPY(buf, "PMAP_Gx");
  buf[6] = '0' + g;
  return buf;
}

bool fer_statPos_pctsByGroup_load(u8 g, const fer_shutterGroupPositionsT positions) {
  char buf[8];
  if (fer_stor_gmBitMask_load(g_to_name(g, buf), (gmBitMaskT*) positions, 1)) {
    pm_setPct(g,0,fer_statPos_getAvgPctGroup(g)); // XXX: enforce new meaning of m==0
    return true;
  }
  return false;
}

bool fer_statPos_pctsByGroup_store(u8 g, fer_shutterGroupPositionsT positions) {
  char buf[8];
  return fer_stor_gmBitMask_save(g_to_name(g, buf), (gmBitMaskT*) positions, 1);
}

