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

#define PCT_UP 100
#define PCT_DOWN 0

#define mv_SIZE 8
struct mv {
  gm_bitmask_t mask;
  u32 start_time;
  bool direction_up : 1; //down=false, up=true
  bool sun_down : 1;
} moving[mv_SIZE];
u8 moving_mask;


static shutterGroupPositionsT pos_map[8];
static u8  pos_map_changed;
enum { pm_GROUP_UNUSED=101, pm_MEMBER_UNUSED, pm_INVALID };
#define pm_getPct(g,m) (pos_map[(g)][(m)]+0)
#define pm_setPct(g,m,pct) pos_map[(g)][(m)] = (pct)
#define pm_setMemberUnused(g,m) pm_setPct((g),(m),pm_MEMBER_UNUSED)
#define pm_isMemberUnused(g, m) (pm_getPct((g),(m)) == pm_MEMBER_UNUSED)
#define pm_setGroupUnused(g) pm_setPct((g),0,pm_GROUP_UNUSED)
#define pm_isGroupUnused(g) (pm_getPct((g),0) == pm_GROUP_UNUSED)

static bool ferPos_mCalcSunStop(u8 g, u8 m, u16 time_s10);
static bool ferPos_mSunReadyToMoveDown(u8 g, u8 m);


static int 
get_state(u32 a, int g, int m) {
  precond(0 <= g && g <= 7 && 0 <= m && m <= 7);

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

  return 0;
}

int 
ferPos_mGetPct(u32 a, u8 g, u8 m) {
  precond(g <= 7 && m <= 7);

  return get_state(a, g, m);
}


int 
ferPos_mSetPct(u32 a, u8 g, u8 m, u8 pct) {
  int position = pct;
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (pair_getControllerPairings(a, &gm))
    for (g=1; g <= GRP_MAX; ++g) {
      for (m=1; m <= MBR_MAX; ++m) {
        if (gm_GetBit(gm, g, m)) {
          // recursion for each paired g/m
          ferPos_mSetPct(0, g, m, pct);
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
ferPos_mmSetPcts(gm_bitmask_t mm, u8 p) {
  u8 g, m;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (GET_BIT(mm[g], m)) {
        ferPos_mSetPct(0, g, m, p);
      }
    }
  }
  return 0;
}

int 
ferPos_printAllPcts() {
  u8 g, m, g2, m2;
  gm_bitmask_t msk = {0,};

  so_output_message(SO_POS_begin, 0);
  for (g=0; g < 8; ++g) {
    if (pm_isGroupUnused(g))
      continue;
    for (m=0; m < 8; ++m) {
      if (pm_isMemberUnused(g,m))
        continue; //
      if (gm_GetBit(msk, g, 0) || gm_GetBit(msk, g, m))
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

extern volatile u32 run_time_s10;
#define run_time_10(x) (run_time_s10 + 0)

static void start_new_moving_mm(int mvi, gm_bitmask_t mm, u32 rt, bool direction) {
  u8 g;

  moving[mvi] = (struct mv){};

  for (g = 1; g <= GRP_MAX; ++g) {
    moving[mvi].mask[g] = mm[g];
  }
  moving[mvi].direction_up = direction;
  SET_BIT(moving_mask, mvi);
  moving[mvi].start_time = rt;
}

static void start_new_sunDownMoving_mm(int mvi, gm_bitmask_t mm, u32 rt) {
  u8 g, m;

  moving[mvi] = (struct mv ) { };

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (gm_GetBit(mm, g, m)) {
        if (!ferPos_mSunReadyToMoveDown(g, m))
          gm_ClrBit(mm, g, m);
      }
    }
    moving[mvi].mask[g] = mm[g];
  }
  moving[mvi].direction_up = false;
  moving[mvi].sun_down = true;
  SET_BIT(moving_mask, mvi);
  moving[mvi].start_time = rt;
}

static void stop_moving(int mvi, u8 g, u8 m, u32 rt) {
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

#ifdef UNUSED_STATIC_FUNCTION
static void stop_moving_mm(int mvi, gm_bitmask_t mm, u32 rt) {
  struct mv *mv = &moving[mvi];
  int m, g;
  bool remaining = false;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (!gm_GetBit(mm, g, m))
        continue;

      gm_ClrBit(moving[mvi].mask, g, m);
      ferPos_mSetPct(0, g, m, ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, rt - mv->start_time));  // update pct table now
    }
    if (mv->mask[g]) {
      remaining = true;
    }
  }
  if (!remaining) {
    CLR_BIT(moving_mask, mvi);
  }
}
#endif

static int add_to_existing_movement_mm(gm_bitmask_t mm, u32 rt, bool direction) {
  u8 mvi, g;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi) && direction == moving[mvi].direction_up && rt == moving[mvi].start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        moving[mvi].mask[g] |= mm[g];
      }
      return mvi;
    }
  }
  return -1;
}

static int add_to_existing_sunDownMovement_mm(gm_bitmask_t mm, u32 rt) {
  u8 mvi, g, m;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi) && moving[mvi].sun_down && rt == moving[mvi].start_time) {
      // add to an in-use bitmask, if having same start_time and direction
      for (g = 1; g <= GRP_MAX; ++g) {
        for (m = 1; m <= MBR_MAX; ++m) {
          if (gm_GetBit(mm, g, m)) {
            if (!ferPos_mSunReadyToMoveDown(g, m))
              gm_ClrBit(mm, g, m);
          }
        }
        moving[mvi].mask[g] |= mm[g];
      }
      return mvi;
    }
  }
  return -1;
}

static int add_to_new_movement_mm(gm_bitmask_t mm, u32 rt, bool direction) {
  u8 mvi;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    start_new_moving_mm(mvi, mm, rt, direction);
    return mvi;
  }
  return -1;
}

static int add_to_new_sunMovement_mm(gm_bitmask_t mm, u32 rt) {
  u8 mvi;

  for (mvi = 0; mvi < mv_SIZE; ++mvi) {
    if (GET_BIT(moving_mask, mvi))
      continue;

    start_new_sunDownMoving_mm(mvi, mm, rt);
    return mvi;
  }
  return -1;
}

// register moving related commands sent to a shutter to keep track of its changing position
int 
ferPos_mmMove(gm_bitmask_t mm, fer_cmd cmd) {
  u32 rt = run_time_10(0);
  int mvi;
  u8 g, m;

  bool isMoving = false, direction = false, isStopping = false, isSun = false;

  switch (cmd) {
  case fer_cmd_STOP:
    isStopping = true;
    break;
  case fer_cmd_SunUP:
    isSun = true;
  case fer_cmd_UP:
    isMoving = true;
    direction = 1;
    break;
  case fer_cmd_SunDOWN:
    isSun = true;
  case fer_cmd_DOWN:
    isMoving = true;
    direction = 0;
    break;
  default:
    return -1;
    break;

  }

  if (isMoving) {
    for (g = 1; g <= GRP_MAX; ++g) {
      for (m = 1; m <= MBR_MAX; ++m) {
        if (gm_GetBit(mm, g, m)) {
          u8 currPct = get_state(0, g, m);
          if ((direction && currPct == PCT_UP) || (!direction && currPct == PCT_DOWN))
            gm_ClrBit(mm, g, m); // cannot move beyond end point
        }
      }
    }
  }

  if (moving_mask && (isStopping || isMoving)) {
    for (mvi = 0; mvi < mv_SIZE; ++mvi) {
      struct mv *mv = &moving[mvi];

      if (!GET_BIT(moving_mask, mvi))
        continue;

      for (g = 1; g <= GRP_MAX; ++g) {
        if (!mv->mask[g] || !mm[g])
          continue;
        for (m = 1; m <= MBR_MAX; ++m) {
          if (!gm_GetBit(mv->mask, g, m))
            continue;
          if (!gm_GetBit(mm, g, m))
            continue;

          if (isMoving && mv->direction_up == direction) {
            gm_ClrBit(mm, g, m);// already moving in right direction (XXX: is writing to argument ok?)
            continue;
          }

          stop_moving(mvi, g, m, rt);
        }
      }
    }
  }

  if (isMoving) {
    if (isSun && !direction) {  // XXX: if sun-up check if we are in sun-down position
      if (add_to_existing_sunDownMovement_mm(mm, rt) < 0) {
        add_to_new_sunMovement_mm(mm, rt);
      }
    } else {
      if (add_to_existing_movement_mm(mm, rt, direction) < 0) {
        add_to_new_movement_mm(mm, rt, direction);
      }
    }

  }

  return 0;
}


int 
ferPos_mMove(u32 a, u8 g, u8 m, fer_cmd cmd) {
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (pair_getControllerPairings(a, &gm))
      return ferPos_mmMove(gm, cmd);
    return 0;
  }
#endif

  gm_bitmask_t mm = { 0, };
  if (g == 0) {
    for (g = 1; g <= GRP_MAX; ++g) {
      mm[g] = 0xfe;
    }
  } else if (m == 0) {
    mm[g] = 0xfe;
  } else {
    gm_SetBit(mm, g, m);
  }

  return ferPos_mmMove(mm, cmd);
}



#define DEF_MV_UP_10 260
#define DEF_MV_DOWN_10 250
#define DEF_MV_SUN_DOWN_10 100

struct shutter_timings st_def = { DEF_MV_UP_10, DEF_MV_DOWN_10, DEF_MV_SUN_DOWN_10 };


u16 ferPos_mCalcMoveDuration_fromPctDiff(u8 g, u8 m, u8 curr_pct, u8 pct) {
  bool direction = curr_pct < pct;
  u8 pct_diff = direction ? pct - curr_pct : curr_pct - pct;
  struct shutter_timings st = st_def;
  ferPos_prefByM_load(&st, g, m);

  if (st.move_up_tsecs == 0)
     st.move_up_tsecs = st_def.move_up_tsecs;

  if (st.move_down_tsecs == 0)
     st.move_down_tsecs = st_def.move_down_tsecs;


  u16 result = (direction ? pct_diff * st.move_up_tsecs : pct_diff * st.move_down_tsecs) / 100;
#if 0
  if (curr_pct == 0 && direction)
    result += MV_UP_TO1;
  else if (pct == 0 && !direction)
    result += MV_DOWN_FROM1;
#endif
    return result;
}


u8 ferPos_mCalcMovePct_fromDirectionAndDuration(u8 g, u8 m, bool direction_up, u16 time_s10) {
  int pct = get_state(0, g, m);
  struct shutter_timings st = st_def;
  ferPos_prefByM_load(&st, g, m);

  if (st.move_up_tsecs == 0)
     st.move_up_tsecs = st_def.move_up_tsecs;

  if (st.move_down_tsecs == 0)
     st.move_down_tsecs = st_def.move_down_tsecs;

  if (pct == -1) {
    return direction_up ? PCT_UP : PCT_DOWN;// XXX
  }

  int add = (direction_up ? 100 * time_s10 /  st.move_up_tsecs : -100 * time_s10 / st.move_down_tsecs);
  add += pct;

  if (add > 100)
    return 100;
  else if (add < 0)
    return 0;
  else
    return add;
}

static bool ferPos_mCalcSunStop(u8 g, u8 m, u16 time_s10) {
  int pct = get_state(0, g, m);

  if (pct != 100) {
    return true; // XXX: sun-down must start at 100%
  }

  struct shutter_timings st = st_def;
  ferPos_prefByM_load(&st, g, m);

  if (st.move_sundown_tsecs == 0)
     st.move_sundown_tsecs = st_def.move_sundown_tsecs;

  return (time_s10 > st.move_sundown_tsecs);
}

static bool ferPos_mSunReadyToMoveDown(u8 g, u8 m) {

  if (get_state(0, g, m) != PCT_UP)
    return false;

  if (gm_GetBit(manual_bits, g, m))
    return false;

  if (ferPos_mCalcSunStop(g, m, 0))
    return false;

  timer_data_t td = {};
  if (read_timer_data(&td, &g, &m, true)) {
    if (!td_is_sun_auto(&td))
      return false;
  }

  return true;
}

int ferPos_mGetMovingPct(u32 a, u8 g, u8 m) {
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    return -1;
  }

  u32 rt = run_time_10(0);
  u8 i;
  u8 mask = moving_mask;

  for (i = 0; mask != 0; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;

    struct mv *mv = &moving[i];

    if (gm_GetBit(mv->mask, g, m)) {
      u16 time_s10 = rt - mv->start_time;
      u8 pct = ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, time_s10);
      return pct;
    }
  }
  return ferPos_mGetPct(a, g, m);
}

static void ferPos_markMovingAsComplete(struct mv *mv, u8 g, u8 m, u8 pct) {
  ferPos_mSetPct(0, g, m, pct);
  gm_ClrBit(mv->mask, g, m);
}

static void ferPos_printMovingPct(u8 g, u8 m, u8 pct) {
  if (mutex_cliTake()) {
    so_arg_gmp_t gmp = { g, m, pct };
    if (sj_open_root_object("tfmcu")) {
      so_output_message(SO_POS_PRINT_GMP, &gmp);
      sj_close_root_object();
      cli_print_json(sj_get_json());
    }
    mutex_cliGive();
  }
}

// check if a moving shutter has reached its end position
static void ferPos_mvCheck_mvi(u8 mvi) {
  u8 g, m, pct;

  struct mv *mv = &moving[mvi];
  u16 time_s10 = run_time_10(0) - mv->start_time;
  bool remaining = false;

  for (g = 1; g < 8; ++g) {
    for (m = 1; m < 8; ++m) {
      if (!gm_GetBit(mv->mask, g, m))
        continue;

      pct = ferPos_mCalcMovePct_fromDirectionAndDuration(g, m, mv->direction_up, time_s10);

      if ((mv->direction_up && pct == PCT_UP) || (!mv->direction_up && pct == PCT_DOWN)) {
        ferPos_markMovingAsComplete(mv, g, m, pct);
        continue;
      }

      if (mv->sun_down && ferPos_mCalcSunStop(g, m, time_s10)) {
        ferPos_markMovingAsComplete(mv, g, m, pct);
        continue;
      }

      remaining = true;
      ferPos_printMovingPct(g, m, pct);
    }
  }

  if (!remaining)
    CLR_BIT(moving_mask, mvi);
}

static void ferPos_mvCheck() {
  u8 i;
  u8 mask = moving_mask;

  for (i = 0; mask != 0; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;

    ferPos_mvCheck_mvi(i);
  }
}


static void ferPos_autoSavePositions_iv(int interval_ts) {
  static u32 next_save_pos;
  if (pos_map_changed && next_save_pos < run_time_10()) {
    next_save_pos = run_time_10() + interval_ts;
    u8 g, mask = pos_map_changed;
    pos_map_changed = 0;

    for (g=0; mask; ++g, (mask >>=1)) {
      if (mask&1)
        ferPos_pctsByGroup_store(g, pos_map[g]);
    }
  }
}

static void ferPos_checkMv_iv(int interval_ts) {
  static int next_s10;
  if (moving_mask && next_s10 < run_time_10()) {
    next_s10 = run_time_10() + interval_ts;
    ferPos_mvCheck();
  }
}

void ferPos_loop() {
  ferPos_checkMv_iv(20);
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

