#include <string.h>

#include "fernotron_pos/shutter_pct.h"
#include "fernotron_pos/shutter_prefs.h"

#include "app_config/proj_app_cfg.h"

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
#include "misc/time/run_time.h"

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

#define GRP_COUNT 7
#define MBR_COUNT 7
#define GRP_MAX 7
#define MBR_MAX 7


static shutterGroupPositionsT pos_map[8];
static u8  pos_map_changed;

enum { pm_GROUP_UNUSED=101, pm_MEMBER_UNUSED, pm_INVALID };
#define pm_getPct(g,m) (pos_map[(g)][(m)]+0)
#define pm_setPct(g,m,pct) pos_map[(g)][(m)] = (pct)
#define pm_setMemberUnused(g,m) pm_setPct((g),(m),pm_MEMBER_UNUSED)
#define pm_isMemberUnused(g, m) (pm_getPct((g),(m)) == pm_MEMBER_UNUSED)
#define pm_setGroupUnused(g) pm_setPct((g),0,pm_GROUP_UNUSED)
#define pm_isGroupUnused(g) (pm_getPct((g),0) == pm_GROUP_UNUSED)


int
ferPos_getPct(u32 a, u8 g, u8 m) {
  precond(g <= 7 && m <= 7);

  if (m != 0 && !pm_isGroupUnused(g) && !pm_isMemberUnused(g,0))
    return pm_getPct(g, 0);

  if (!pm_isMemberUnused(g,m))
    return pm_getPct(g,m);

  return -1;
}

static int 
set_state(u32 a, int g, int m, int position) {
  u8  mi;
  DT(ets_printf("%s: a=%lx, g=%d, m=%d, position=%d\n", __func__, a, g, m, position));
  precond(0 <= g && g <= 7 && 0 <= m && m <= 7);
  precond(0 <= position && position <= 100);

  if (m != 0 && !pm_isGroupUnused(g) && !pm_isMemberUnused(g,0)) {
    u8 pct = pm_getPct(g,0);

    for (mi = 1; mi <= MBR_MAX; ++mi) {
      pm_setPct(g,mi, pct);
    }
  }

  pm_setMemberUnused(g,0);
  pm_setPct(g,m,position);

  SET_BIT(pos_map_changed, g);
  fpos_POSITIONS_UNSAVED_cb();
  return 0;
}

int 
ferPos_setPct(u32 a, u8 g, u8 m, u8 pct) {
  int position = pct;
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (pair_getControllerPairings(a, &gm))
    for (g=1; g <= GRP_MAX; ++g) {
      for (m=1; m <= MBR_MAX; ++m) {
        if (gm_GetBit(&gm, g, m)) {
          // recursion for each paired g/m
          ferPos_setPct(0, g, m, pct);
        }
      }
    }
    return 0;
  }
#endif

  if (0 <= position && position <= 100) {
    if (a == 0 || a == C.fer_centralUnitID) {
      so_arg_gmp_t gmp = { g, m, position };
      if (mutex_cliTake()) {
        if (sj_open_root_object("tfmcu")) {
          so_output_message(SO_POS_PRINT_GMP, &gmp);  // FIXME: better use bit mask?
          sj_close_root_object();
          cli_print_json(sj_get_json());
        }
        mutex_cliGive();
      }
    }
  }

  if (position < 0)
    return -1;
  else
    return set_state(a, g, m, position);
}

int 
ferPos_setPcts(gm_bitmask_t *mm, u8 p) {
  u8 g, m;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (gm_GetBit(mm, g, m)) {
        ferPos_setPct(0, g, m, p);
      }
    }
  }
  return 0;
}

int 
ferPos_printPctsAll() {
  u8 g, m, g2, m2;
  gm_bitmask_t msk = {0,};

  so_output_message(SO_POS_begin, 0);
  for (g=0; g < 8; ++g) {
    if (pm_isGroupUnused(g))
      continue;
    for (m=0; m < 8; ++m) {
      if (pm_isMemberUnused(g,m))
        continue; //
      if (gm_GetBit(&msk, g, 0) || gm_GetBit(&msk, g, m))
        continue; // was already processed by a previous pass

      u8 pct = pm_getPct(g,m);
      gm_bitmask_t pos_msk = {0,};
      for (g2=g; g2 < 8; ++g2) {
        for (m2=0; m2 < 8; ++m2) {
          if (pm_getPct(g2,m2) == pct) {
            gm_SetBit(&pos_msk, g2, m2); // mark as being equal to pct
            gm_SetBit(&msk, g2, m2); // mark as already processed
            if (m2 == 0) {
              goto next_g2;  // all group members have the same pct value because m==0 is used
            }
          }
        }
        next_g2:;
      }

      so_arg_mmp_t mmp = { &pos_msk, pct };
      so_output_message(SO_POS_PRINT_MMP, &mmp);
    }
  }
  so_output_message(SO_POS_end, 0);
  return 0;
}

static void ferPos_autoSavePositions_iv(int interval_ts) {
  static u32 next_save_pos;
  if (pos_map_changed && next_save_pos < get_now_time_ts()) {
    next_save_pos = get_now_time_ts() + interval_ts;
    u8 g, mask = pos_map_changed;
    pos_map_changed = 0;

    for (g=0; mask; ++g, (mask >>=1)) {
      if (mask&1)
        ferPos_pctsByGroup_store(g, pos_map[g]);
    }
    fpos_POSTIONS_SAVED_cb();
  }
}


void ferPos_loop() {
  ferPos_checkStatus_whileMoving_periodic(20);
  ferPos_autoSavePositions_iv(100);
}

void ferPos_loopAutoSave() {
  ferPos_autoSavePositions_iv(100);
}

void ferPos_init() {
  u8 g, m;

  for (g=0; g < 8; ++g) {
    ferPos_pctsByGroup_load(g, pos_map[g]);
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
  strcpy(buf, "PMAP_Gx");
  buf[6] = '0' + g;
  return buf;
}

int ferPos_pctsByGroup_load(u8 g, const shutterGroupPositionsT positions) {
  char buf[8];
  return fer_gmByName_load(g_to_name(g, buf), (gm_bitmask_t*) positions, 1);
}

int ferPos_pctsByGroup_store(u8 g, shutterGroupPositionsT positions) {
  char buf[8];
  return fer_gmByName_store(g_to_name(g, buf), (gm_bitmask_t*) positions, 1);
}

