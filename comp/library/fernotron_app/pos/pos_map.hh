/**
 * \file fernotron_app/pos/pos_map.hh
 */

#include <fernotron/gm_set.hh>
#include <fernotron/pos/positions_static.h>
#include <fernotron/fer_main.h>

#include <utils_time/periodic.h>

#include "move.h"

enum { pm_GROUP_UNUSED=101, pm_MEMBER_UNUSED, pm_INVALID };

#define GRP_MAX 7
#define MBR_MAX 7
#define DT(x)
#define D(x)

#define CFG_NAMESPACE "Tronferno"

static inline char *g_to_name(u8 g, char *buf) {
  STRCPY(buf, "GMBM_PMAP_Gx");
  buf[11] = '0' + g;
  return buf;
}


class Fer_Pos_Map {
  struct __attribute__ ((packed)) posDataT {
    operator uint8_t() { return pct; }
    bool isGroupUnused() const { return pct == pm_GROUP_UNUSED; }
    bool isMemberUnused() const { return pct == pm_MEMBER_UNUSED; }
    bool isInvalid() const { return pct == pm_INVALID; }
    bool isValidPos() const { return 0 <= pct && pct <= 100; }
    bool isSunPos() const { return isValidPos() && sun; }
    uint8_t pct : 7;
    bool sun : 1;
  };
  posDataT allPos_[8][8];
  u8 changedGroupMask_ = 0;

public:
  Fer_Pos_Map() {
    u8 g, m;

    for (g=0; g < 8; ++g) {
      allPos_[g][0].pct = pm_GROUP_UNUSED;
      for (m=1; m < 8; ++m) {
        allPos_[g][m].pct = pm_MEMBER_UNUSED;
      }
    }
  }


public:
  u8 getChangedGroups()  { u8 result = changedGroupMask_; changedGroupMask_ = 0; return result; }

  u8 getPct(u8 g, u8 m) {
       u8 result = (allPos_[g][m] + 0);
       if (result == pm_INVALID && m == 0) {
         allPos_[g][0].pct = result = getAvgPctGroup(g);
       }
       return result;
  }

  bool isSunPos(u8 g, u8 m) {
       return allPos_[g][m].isSunPos();
  }

  void setPct(u8 g, u8 m, u8 pct, bool  isSunPos = false) {
    allPos_[g][m].pct = (pct);
    allPos_[g][m].sun = isSunPos;
    SET_BIT(changedGroupMask_, g);
    allPos_[g][0].pct = pm_INVALID;
  }
  void setMemberUnused(u8 g, u8 m) {
    setPct(g, m, pm_MEMBER_UNUSED);
    SET_BIT(changedGroupMask_, g);
  }
  bool isMemberUnused(u8 g, u8 m) {
    return (getPct(g, m) == pm_MEMBER_UNUSED);
  }
  void setGroupUnused(u8 g) {
    setPct(g, 0, pm_GROUP_UNUSED);
    SET_BIT(changedGroupMask_, g);
  }
  bool isGroupUnused(u8 g) {
    return (getPct(g, 0) == pm_GROUP_UNUSED);
  }

  void autoSavePositions() {
    DT(ets_printf("%s: \n", __func__, interval_ts));

    fer_pos_POSITIONS_SAVE_cb(false);
    u8 mask = changedGroupMask_;
    changedGroupMask_ = 0;
    u8 g;
    for (g = 0; mask; ++g, (mask >>= 1)) {
      if ((mask & 1) == 0)
        continue;

      store_group_positions(g);
      D(io_printf_v(vrbDebug, "autosave_pos: g=%d\n", (int)g));
    }

  }


  void autoSavePositions_iv(int interval_ts) {
    DT(ets_printf("%s: interval_tx=%d\n", __func__, interval_ts));
    static unsigned next_save_pos;
    u8 mask = changedGroupMask_;
    if (mask && periodic_ts(interval_ts, &next_save_pos)) {
      autoSavePositions();
      next_save_pos = 0;
    }
  }



  static bool store_load(const char *name, posDataT *gm);
  static bool store_save(const char *name, const posDataT *gm);

  bool load_group_positions(u8 g) {
    char buf[16];
    if (store_load(g_to_name(g, buf), allPos_[g])) {
      setPct(g,0,getAvgPctGroup(g)); // XXX: enforce new meaning of m==0
      return true;
    }
    return false;
  }

  bool store_group_positions(u8 g) {
    char buf[16];
    return store_save(g_to_name(g, buf), allPos_[g]);
  }

  u8 getAvgPctGroup(u8 g) {
    u8 m;
    int count=0, sum=0;
    for (m = 1; m <= MBR_MAX; ++m) {
      if (isMemberUnused(g, m))
        continue;
      if (!fer_usedMemberMask.getMember(g,m))
        continue;
      ++count;
      sum += getPct(g, m);
    }
    if (count) {
      return sum/count;
    }
    return pm_GROUP_UNUSED;
  }

  void updateAvgPcts(bool force = false) {
    for (u8 g = 0; g <= GRP_MAX; ++g) {
      if (!force && !GET_BIT(changedGroupMask_, g))
        continue;
          setPct(g, 0, getAvgPctGroup(g));
    }
  }

  Fer_GmSet getValidPcts() {
    Fer_GmSet result;

    for (gT g = 1; g <= GRP_MAX; ++g) {
      if (isGroupUnused(g))
        continue;
      for (mT m = 0; m <= MBR_MAX; ++m) {
        if (isMemberUnused(g, m))
          continue; //
        if (!allPos_[g][m].isValidPos())
          continue;
        result.setMember(g, m);
      }
    }
    return result;
  }


  template<class FuncObj>
  void fer_statPos_forEachPct(const FuncObj &func_obj) {
    const Fer_GmSet all = (getValidPcts() & fer_usedMemberMask).updateGroup();
    Fer_GmSet seen;

    for (auto ia = all.begin(1); ia; ++ia) {
      const gT g = ia.getG();
      const mT m = ia.getM();

      if (seen.getMember(g, m))
        continue; // was already processed by a previous pass

      u8 pct = getPct(g, m);
      Fer_GmSet same_pct;
      for (auto it = all.begin(g); it; ++it) {
        const gT g2 = it.getG();
        const mT m2 = it.getM();

        if (getPct(g2, m2) == pct) {
          same_pct.setMember(g2, m2); // mark as being equal to pct
          seen.setMember(g2, m2); // mark as already processed
        }
      }
      func_obj(same_pct, pct);
    }
  }
};





#undef D
#undef DT
