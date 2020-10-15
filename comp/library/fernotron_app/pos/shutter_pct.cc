#include "app_config/proj_app_cfg.h"

#include <string.h>

#include "fernotron/pos/shutter_pct.h"
#include "fernotron/pos/shutter_prefs.h"

#include "app_settings/config.h"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "app_uout/status_output.h"
#include "app_uout/callbacks.h"
#include "uout/status_json.hh"
#include "uout/cli_out.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "utils_time/run_time.h"
#include "utils_time/periodic.h"

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


#define cfg_isMemberUnused(g,m) !C.fer_usedMemberMask.getMember(g, m)

static char *g_to_name(u8 g, char *buf);

void (*fer_pos_POSITIONS_SAVE_cb)(bool has_unsaved);
static inline void fer_pos_POSITIONS_UNSAVED_cb() {
  if (fer_pos_POSITIONS_SAVE_cb)
    fer_pos_POSITIONS_SAVE_cb(true);
}
static inline void fer_pos_POSTIONS_SAVED_cb() {
  if (fer_pos_POSITIONS_SAVE_cb)
    fer_pos_POSITIONS_SAVE_cb(false);
}

enum { pm_GROUP_UNUSED=101, pm_MEMBER_UNUSED, pm_INVALID };

static class Fer_Pos_Map {
  fer_shutterGroupPositionsT allPos_[8];
  u8 changedGroupMask_;

public:
  fer_shutterGroupPositionsT & operator[](int idx) { return allPos_[idx]; }
  u8 getChangedGroups()  { u8 result = changedGroupMask_; changedGroupMask_ = 0; return result; }

  inline u8 getPct(u8 g, u8 m) { return (allPos_[(g)][(m)]+0); }
  inline void setPct(u8 g, u8 m, u8 pct) { allPos_[(g)][(m)] = (pct); SET_BIT(changedGroupMask_, g); }
  inline void setMemberUnused(u8 g, u8 m) { setPct((g),(m),pm_MEMBER_UNUSED); SET_BIT(changedGroupMask_, g); }
  inline bool isMemberUnused(u8 g, u8 m) { return (getPct((g),(m)) == pm_MEMBER_UNUSED); }
  inline void setGroupUnused(u8 g) { setPct((g),0,pm_GROUP_UNUSED); SET_BIT(changedGroupMask_, g); }
  inline bool isGroupUnused(u8 g) { return (getPct((g),0) == pm_GROUP_UNUSED); }


  void fer_pos_autoSavePositions_iv(int interval_ts) {
    DT(ets_printf("%s: interval_tx=%d\n", __func__, interval_ts));
    static unsigned next_save_pos;
    u8 mask = changedGroupMask_;
    if (mask && periodic_ts(interval_ts, &next_save_pos)) {
      fer_pos_POSTIONS_SAVED_cb();
      changedGroupMask_ = 0;
      next_save_pos = 0;

      u8 g;
      for (g = 0; mask; ++g, (mask >>= 1)) {
        if ((mask & 1) == 0)
          continue;

        store_group_positions(g);
        D(io_printf_v(vrbDebug, "autosave_pos: g=%d\n", (int)g));
      }
    }
  }

  bool load_group_positions(u8 g) {
    char buf[8];
    if (fer_stor_gmSet_load(g_to_name(g, buf), &allPos_[g].grpPos_, 1)) {
      setPct(g,0,fer_statPos_getAvgPctGroup(g)); // XXX: enforce new meaning of m==0
      return true;
    }
    return false;
  }

  bool store_group_positions(u8 g) {
    char buf[8];
    return fer_stor_gmSet_save(g_to_name(g, buf), &allPos_[g].grpPos_, 1);
  }

  int fer_statPos_getAvgPctGroup(u8 g) {
    u8 m;
    int count=0, sum=0;
    for (m = 1; m <= MBR_MAX; ++m) {
      if (isMemberUnused(g, m))
        continue;
      if (cfg_isMemberUnused(g,m))
        continue;
      ++count;
      sum += getPct(g, m);
    }
    if (count) {
      return sum/count;
    }
    return pm_GROUP_UNUSED;
  }

} pos_map;



int
fer_statPos_getPct(u32 a, u8 g, u8 m) {
  precond(g <= 7 && m <= 7);

  if (g == 0) {
    return -1; // TODO: average all?
  } else {
    if (pos_map.isMemberUnused(g, m))
      return -1;
    return pos_map.getPct(g, m);
  }
  return -1;
}

static int 
set_state(u32 a, int g, int m, int position) {
  DT(db_printf("%s: a=%x, g=%d, m=%d, position=%d\n", __func__, a, g, m, position));
  precond(0 <= g && g <= 7 && 0 <= m && m <= 7);
  precond(0 <= position && position <= 100);

  pos_map.setMemberUnused(g,0);
  pos_map.setPct(g,m,position);
  pos_map.setPct(g,0, pos_map.fer_statPos_getAvgPctGroup(g));

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
    Fer_GmSet gm;
    if (fer_alias_getControllerPairings(a, &gm))
      for (Fer_GmSet_Iterator it(1,1,true); it; ++it) {
        const gT g = it.getG();
        const mT m = it.getM();
        if (gm.getMember(g, m)) {
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

      so_arg_gmp_t gmp[3] = { { g, m, pct }, { g, 0, (u8) pos_map.getPct(g, 0) }, { 0xff, 0xff, 0xff } };
      uoApp_publish_pctChange_gmp(gmp, 2);
    }
  }
#endif

  return result;
}

int fer_statPos_setPcts(Fer_GmSet *mm, u8 p) {
  for (Fer_GmSet_Iterator it(1, 1, true); it; ++it) {
    const gT g = it.getG();
    const mT m = it.getM();
    if (mm->getMember(g, m)) {
      fer_statPos_setPct(0, g, m, p);
    }
  }
  return 0;
}

int 
fer_statPos_printAllPcts(const struct TargetDesc &td) {
  Fer_GmSet msk;

  soMsg_pos_begin(td);
  for (gT g=1; g < 8; ++g) {
    if (pos_map.isGroupUnused(g))
      continue;
    for (mT m=0; m < 8; ++m) {
      if (pos_map.isMemberUnused(g,m))
        continue; //
      if (m != 0 && cfg_isMemberUnused(g,m))
        continue; //

      if (msk.getMember(g, m))
        continue; // was already processed by a previous pass

      u8 pct = pos_map.getPct(g,m);
      Fer_GmSet pos_msk;
      for (Fer_GmSet_Iterator it(g); it; ++it) {
        const gT g2 = it.getG();
        const mT m2 = it.getM();

        if (pos_map.getPct(g2,m2) == pct) {
          if (m2 != 0 && cfg_isMemberUnused(g2, m2))
            continue;
          pos_msk.setMember(g2, m2); // mark as being equal to pct
          msk.setMember(g2, m2); // mark as already processed
        }
      }
      so_arg_mmp_t mmp = { &pos_msk, pct };
      soMsg_pos_print_mmp(td, mmp);
    }
  }
  soMsg_pos_end(td);
  return 0;
}



void fer_pos_loop() {
  fer_pos_checkStatus_whileMoving_periodic(20);
  pos_map.fer_pos_autoSavePositions_iv(100);
}

void fer_statPos_loopAutoSave() {
  pos_map.fer_pos_autoSavePositions_iv(100);
}

void fer_pos_init() {
  u8 g, m;

  for (g=0; g < 8; ++g) {
    pos_map.load_group_positions(g);
    if (pos_map.getPct(g,0) >= pm_INVALID)
      pos_map.setGroupUnused(g);
    for (m=1; m < 8; ++m) {
      if (pos_map.getPct(g,m) >= pm_INVALID)
        pos_map.setMemberUnused(g,m);
    }

  }
}

typedef u8 u8a8[8];


static char *g_to_name(u8 g, char *buf) {
  STRCPY(buf, "PMAP_Gx");
  buf[6] = '0' + g;
  return buf;
}



