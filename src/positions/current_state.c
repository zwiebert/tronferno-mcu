#include "user_config.h"

#include "positions/current_state.h"
#include "config/config.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include "userio/status_output.h"
#include "main/pairings.h"

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

enum pos {
  POS_0, POS_50, POS_100, POS_SIZE
};

static  u8 pos_map[8][8];
#define pm_GROUP_UNUSED 101
#define pm_MEMBER_UNUSED 102
#define pm_getPct(g,m) (pos_map[(g)][(m)]+0)
#define pm_setPct(g,m,pct) pos_map[(g)][(m)] = (pct)
#define pm_setMemberUnused(g,m) pm_setPct((g),(m),pm_MEMBER_UNUSED)
#define pm_isMemberUnused(g, m) (pm_getPct((g),(m)) == pm_MEMBER_UNUSED)
#define pm_setGroupUnused(g) pm_setPct((g),0,pm_GROUP_UNUSED)
#define pm_isGroupUnused(g) (pm_getPct((g),0) == pm_GROUP_UNUSED)


static int ICACHE_FLASH_ATTR
get_state(u32 a, int g, int m) {
  precond(1 <= g && g <= 7 && 1 <= m && m <= 7);

  if (m != 0 && !pm_isGroupUnused(g) && !pm_isMemberUnused(g,0))
    return pm_getPct(g, 0);

  if (!pm_isMemberUnused(g,m))
    return pm_getPct(g,m);

  return -1;
}

static int ICACHE_FLASH_ATTR
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


  return 0;
}

int ICACHE_FLASH_ATTR
get_shutter_state(u32 a, u8 g, u8 m) {
  precond(g <= 7 && m <= 7);

  return get_state(a, g, m);
}


int ICACHE_FLASH_ATTR
set_shutter_state(u32 a, u8 g, u8 m, fer_cmd cmd) {
  u8 pct = -1;
  if (cmd == fer_cmd_UP)
    pct = 100;
  else if (cmd == fer_cmd_DOWN)
    pct = 0;
  else if (cmd == fer_cmd_SunDOWN)
    pct = 50;

  return set_shutter_pct(a, g, m, pct);
}


int ICACHE_FLASH_ATTR
set_shutter_pct(u32 a, u8 g, u8 m, u8 pct) {
  int position = pct;
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (read_pairings(&gm, a))
    for (g=1; g <= GRP_MAX; ++g) {
      for (m=1; m <= MBR_MAX; ++m) {
        if (gm_GetBit(gm, g, m)) {
          // recursion for each paired g/m
          set_shutter_pct(0, g, m, pct);
        }
      }
    }
    return 0;
  }
#endif



  if (0 <= position && position <= 100) {
    if (a == 0 || a == C.fer_centralUnitID) {
      so_arg_gmp_t gmp = { g, m, position };
      so_output_message(SO_POS_PRINT_GMP, &gmp);  // FIXME: better use bit mask?
    }
  }

  if (position < 0)
    return -1;
  else
    return set_state(a, g, m, position);
}

int ICACHE_FLASH_ATTR
modify_shutter_positions(gm_bitmask_t mm, u8 p) {
  u8 g, m;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (GET_BIT(mm[g], m)) {
        set_state(0, g, m, p);
      }
    }
  }
  return 0;
}

int ICACHE_FLASH_ATTR
print_shutter_positions() {
  u8 g, m, g2, m2;
  gm_bitmask_t msk = {0,};

  so_output_message(SO_POS_begin, 0);
  for (g=0; g < 8; ++g) {
    if (pm_isGroupUnused(g))
      continue;
    for (m=0; m < 8; ++m) {
      if (pm_isMemberUnused(g,m))
        continue; //
      if (gm_GetBit(msk, g, m))
        continue; // was already processed by a previous pass

      u8 pct = pm_getPct(g,m);
      gm_bitmask_t pos_msk = {0,};
      for (g2=g; g2 < 8; ++g2) {
        for (m2=0; m2 < 8; ++m2) {
          if (pm_getPct(g2,m2) == pct) {
            gm_SetBit(pos_msk, g2, m2); // mark as being equal to pct
            gm_SetBit(msk, g2, m2); // mark as already processed
            if (m2 == 0) {
              goto next_g2;  // all group members have the same pct value because m==0 is used
            }
          }
        }
        next_g2:;
      }

      so_arg_mmp_t mmp = { pos_msk, pct };
      so_output_message(SO_POS_PRINT_MMP, &mmp);
    }
  }
  so_output_message(SO_POS_end, 0);
  return 0;
}

void currentState_init() {
  u8 g, m;

  for (g=0; g < 8; ++g) {
    pm_setGroupUnused(g);
    for (m=1; m < 8; ++m) {
      pm_setMemberUnused(g,m);
    }
  }
}

