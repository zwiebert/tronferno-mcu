#include "fernotron_pos/current_state.h"

#include "app/proj_app_cfg.h"

#include "config/config.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include "userio_app/status_output.h"
#include "userio/status_json.h"
#include "fernotron_alias/pairings.h"
#include "cli_app/cli_imp.h"

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
} moving[mv_SIZE];
u8 moving_mask;


static u8 pos_map[8][8];
static u8  pos_map_changed;
enum { pm_GROUP_UNUSED=101, pm_MEMBER_UNUSED, pm_INVALID };
#define pm_getPct(g,m) (pos_map[(g)][(m)]+0)
#define pm_setPct(g,m,pct) pos_map[(g)][(m)] = (pct)
#define pm_setMemberUnused(g,m) pm_setPct((g),(m),pm_MEMBER_UNUSED)
#define pm_isMemberUnused(g, m) (pm_getPct((g),(m)) == pm_MEMBER_UNUSED)
#define pm_setGroupUnused(g) pm_setPct((g),0,pm_GROUP_UNUSED)
#define pm_isGroupUnused(g) (pm_getPct((g),0) == pm_GROUP_UNUSED)

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
currentState_getShutterPct(u32 a, u8 g, u8 m) {
  precond(g <= 7 && m <= 7);

  return get_state(a, g, m);
}


int 
currentState_setShutterPct(u32 a, u8 g, u8 m, u8 pct) {
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
          currentState_setShutterPct(0, g, m, pct);
        }
      }
    }
    return 0;
  }
#endif



  if (0 <= position && position <= 100) {
    if (a == 0 || a == C.fer_centralUnitID) {
      so_arg_gmp_t gmp = { g, m, position };
      sj_open_root_object("tfmcu");
      so_output_message(SO_POS_PRINT_GMP, &gmp);  // FIXME: better use bit mask?
      sj_close_root_object();
      cli_print_json(sj_get_json());
    }
  }

  if (position < 0)
    return -1;
  else
    return set_state(a, g, m, position);
}

int 
currentState_modifyShutterPositions(gm_bitmask_t mm, u8 p) {
  u8 g, m;

  for (g = 1; g <= GRP_MAX; ++g) {
    for (m = 1; m <= MBR_MAX; ++m) {
      if (GET_BIT(mm[g], m)) {
        currentState_setShutterPct(0, g, m, p);
      }
    }
  }
  return 0;
}

int 
currentState_printShutterPositions() {
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

// register moving related commands sent to a shutter to keep track of its changing position
int 
currentState_mmMove(gm_bitmask_t mm, fer_cmd cmd) {


  u32 rt = run_time_10(0);
  int mvi, gi;
  u8 g, m;

  bool isMoving = false, direction = false, isStopping = false;

  switch (cmd) {
  case fer_cmd_STOP:
    isStopping = true;
    break;
  case fer_cmd_UP:
    isMoving = true;
    direction = 1;
    break;
  case fer_cmd_DOWN:
    isMoving = true;
    direction = 0;
    break;
  default:
    return -1;
    break;

  }

  if (isMoving) {
    for (g=1; g <= GRP_MAX; ++g) {
      for (m=1; m <= MBR_MAX; ++m) {
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
        for (m=1; m <= MBR_MAX; ++m) {
          if (!gm_GetBit(mv->mask, g, m))
            goto next_i;

          if (isMoving && mv->direction_up == direction) {
            return 0; // already moving in right direction
          } else {
            gm_ClrBit(moving[mvi].mask, g, m);
            currentState_setShutterPct(0, g, m, currentState_mvCalcPct(g, m, mv->direction_up, rt - mv->start_time));  // update pct table now
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
        }
      }
      next_i: ;
    }
  }

  if (isMoving) {
    for (mvi = 0; mvi < mv_SIZE; ++mvi) {
      if (GET_BIT(moving_mask, mvi) && direction == moving[mvi].direction_up && rt == moving[mvi].start_time) {
        // add to an in-use bitmask, if having same start_time and direction
        for (g = 1; g <= GRP_MAX; ++g) {
          moving[mvi].mask[g] |= mm[g];
        }
        break;
      }
      if (GET_BIT(moving_mask, mvi))
        continue;

      // add to empty bitmask
      for (g = 1; g <= GRP_MAX; ++g) {
        moving[mvi].mask[g] = mm[g];
      }
      moving[mvi].direction_up = direction;
      SET_BIT(moving_mask, mvi);
      moving[mvi].start_time = rt;
      break;
    }

  }

  return 0;
}


int 
currentState_Move(u32 a, u8 g, u8 m, fer_cmd cmd) {
  precond(g <= 7 && m <= 7);

  DT(ets_printf("%s: a=%lx, g=%d, m=%d, cmd=%d\n", __func__, a, (int)g, (int)m, (int)cmd));
#ifdef USE_PAIRINGS
  if (!(a == 0 || a == C.fer_centralUnitID)) {
    gm_bitmask_t gm;
    if (pair_getControllerPairings(a, &gm))
      for (g = 1; g <= GRP_MAX; ++g) {
        for (m = 1; m <= MBR_MAX; ++m) {
          if (gm_GetBit(gm, g, m)) {
            // recursion for each paired g/m
            currentState_Move(0, g, m, cmd);
          }
        }
      }
    return 0;
  }
#endif


  if (g == 0 || m == 0) {
    gm_bitmask_t mm = { 0, };
    if (g == 0) {
      for (g = 1; g <= GRP_MAX; ++g) {
        mm[g] = 0xfe;
      }
    } else {
      mm[g] = 0xfe;
    }
    return currentState_mmMove(mm, cmd);
  }

  u32 rt = run_time_10(0);
  int mvi, gi;
  bool isMoving = false, direction = false, isStopping = false;

  switch (cmd) {
  case fer_cmd_STOP:
    isStopping = true;
    break;
  case fer_cmd_UP:
    isMoving = true;
    direction = 1;
    break;
  case fer_cmd_DOWN:
    isMoving = true;
    direction = 0;
    break;
  default:
    return -1;
    break;

  }

  if (isMoving) {
    u8 currPct = get_state(a, g, m);
    if ((direction && currPct == PCT_UP) || (!direction && currPct == PCT_DOWN))
      return -1; // cannot move beyond end point
  }

  if (moving_mask && (isStopping || isMoving)) {
    for (mvi = 0; mvi < mv_SIZE; ++mvi) {
      struct mv *mv = &moving[mvi];

      if (!(GET_BIT(moving_mask, mvi) && gm_GetBit(mv->mask, g, m)))
        continue;

      if (isMoving && mv->direction_up == direction) {
        return 0; // already moving in right direction
      } else {
        gm_ClrBit(moving[mvi].mask, g, m);
        currentState_setShutterPct(0, g, m, currentState_mvCalcPct(g, m, mv->direction_up, rt - mv->start_time));  // update pct table now
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
    }
  }

  if (isMoving) {
    for (mvi = 0; mvi < mv_SIZE; ++mvi) {
      if (GET_BIT(moving_mask, mvi) && direction == moving[mvi].direction_up && rt == moving[mvi].start_time) {
        gm_SetBit(moving[mvi].mask, g, m);
        break;
      }

      if (GET_BIT(moving_mask, mvi))
        continue;

      gm_SetBit(moving[mvi].mask, g, m);
      moving[mvi].direction_up = direction;
      SET_BIT(moving_mask, mvi);
      moving[mvi].start_time = rt;
      break;
    }

  }

  return 0;
}



#define DEF_MV_UP_10 260
#define DEF_MV_DOWN_10 250

struct shutter_timings st_def = { DEF_MV_UP_10, DEF_MV_DOWN_10 };

u16 currentState_mvCalcTime10(u8 g, u8 m, u8 curr_pct, u8 pct) {
  bool direction = curr_pct < pct;
  u8 pct_diff = direction ? pct - curr_pct : curr_pct - pct;
  struct shutter_timings st = st_def;
  shuPref_read_timings(&st, g, m);

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


u8 currentState_mvCalcPct(u8 g, u8 m, bool direction_up, u16 time_s10) {
  int pct = get_state(0, g, m);
  struct shutter_timings st = st_def;
  shuPref_read_timings(&st, g, m);

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

int currentState_getMovingPct(u32 a, u8 g, u8 m) {
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
    u16 time_s10 = rt - mv->start_time;
    {
      if (gm_GetBit(mv->mask, g, m)) {
        u8 pct = currentState_mvCalcPct(g, m, mv->direction_up, time_s10);
        return pct;
      }
    }

  }
  return currentState_getShutterPct(a, g, m);
}

// check if a moving shutter has reached its end position
static void currentState_mvCheck() {
  u32 rt = run_time_10(0);
  u8 i, g, m;
  u8 mask = moving_mask;

  for (i = 0; mask != 0; ++i, (mask >>= 1)) {
    if (!(mask & 1))
      continue;
    struct mv *mv = &moving[i];
    u16 time_s10 = rt - mv->start_time;
   {
      bool remaining = false;
      for (g = 1; g < 8; ++g) {
        for (m = 1; m < 8; ++m) {
          if (gm_GetBit(mv->mask, g, m)) {
            u8 pct = currentState_mvCalcPct(g, m, mv->direction_up, time_s10);
            if ((mv->direction_up && pct == PCT_UP) || (!mv->direction_up && pct == PCT_DOWN)) {
              currentState_setShutterPct(0, g, m, pct);
              gm_ClrBit(mv->mask, g, m);
            } else {
              remaining = true;
              so_arg_gmp_t gmp = {g, m, pct};
              sj_open_root_object("tfmcu");
              so_output_message(SO_POS_PRINT_GMP, &gmp);
              sj_close_root_object();
              cli_print_json(sj_get_json());
            }
          }
        }
      }
      if (!remaining) {
        CLR_BIT(moving_mask, i);
      }
    }
  }
}



void currentState_loop() {
  static int next_s10;
  if (moving_mask && next_s10 < run_time_10()) {
    next_s10 = run_time_10() + 20;
    currentState_mvCheck();
  }

  static u32 next_save_pos;
  if (pos_map_changed && next_save_pos < run_time_10()) {
    next_save_pos = run_time_10() + 100;
    u8 g, mask = pos_map_changed;
    pos_map_changed = 0;

    for (g=0; mask; ++g, (mask >>=1)) {
      if (mask&1)
        save_g_positions(g, pos_map[g]);
    }
  }
}

void currentState_init() {
  u8 g, m;

  for (g=0; g < 8; ++g) {
    read_g_positions(g, pos_map[g]);
    if (pm_getPct(g,0) >= pm_INVALID)
      pm_setGroupUnused(g);
    for (m=1; m < 8; ++m) {
      if (pm_getPct(g,m) >= pm_INVALID)
        pm_setMemberUnused(g,m);
    }

  }
}

