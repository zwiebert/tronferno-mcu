/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app/config/proj_app_cfg.h"
#include "timer_data_kvs.h"
#include "app/settings/config.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"
#include "debug/dbg.h"
#include <alloca.h>
#include <string.h>

#ifndef TEST_HOST
#define printf ets_printf
#define DB(x) do { if (TXTIO_IS_VERBOSE(vrbDebug)) { x; } } while(0)
#define DB2(x) DB(x)
#else
#include <stdio.h>
#define DB(x) x
#define DB2(x)
#endif



////////////////// private ///////////////////////////////////////////////////////////////
const char TD_KVS_NAMESPACE[] = "auto_td";

class TdKey {
public:
  constexpr TdKey(u8 g, u8 m) {
    myKey[2] += g;
    myKey[3] += m;
  }
  TdKey() = delete;
  constexpr operator const char*() const { return myKey; }
private:
  char myKey[5] = "td00";
};

static int delete_shadowded_kv(u8 group, u8 memb) {
  int result = 0;
  DB2(printf("delete shadowed files(group=%d, memb=%d)\n", (int)group, (int)memb));

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    for (gm_iterator it; it; ++it) {
      const gT g = it.getG();
      const mT m = it.getM();
      if ((group == 0 || group == g) && (memb == 0 || (memb == m && C.fer_usedMemberMask.getBit(g, m)))) {
        if (kvs_erase_key(handle, TdKey(g, m))) {
          DB2(printf("shadow deleted: g=%d, m=%d\n", (int)g, (int)m));
          ++result;
        }
      }

    }
    kvs_commit(handle);
    kvs_close(handle);
  }
  return result;
}


////////////////////////////////// public ////////////////////////////////////////////////////////////////////

bool erase_timer_data_kvs(u8 g, u8 m) {
  precond (g <= 7 && m <= 7);
  return delete_shadowded_kv(g, m) > 0;
}

bool  save_timer_data_kvs(Fer_TimerData *p, u8 g, u8 m) {
  bool success = false;
  precond(p && g <= 7 && m <= 7);

  delete_shadowded_kv(g, m);

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    success = kvs_rw_blob(handle, TdKey(g, m), p, sizeof(*p), true);
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;

}

bool  read_timer_data_kvs(Fer_TimerData *p, u8 *gp, u8 *mp, bool wildcard) {
  bool success = false;
  precond(p && gp && mp && *gp <= 7 && *mp <= 7);


  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_READ);
  if (handle) {
    success = kvs_rw_blob(handle,  TdKey(*gp, *mp), p, sizeof(*p), false);

    if (!success && wildcard) {
      if ((success = kvs_rw_blob(handle, TdKey(*gp, 0), p, sizeof(*p), false))) {
        *mp = 0;
      } else if ((success = kvs_rw_blob(handle, TdKey(0, *mp), p, sizeof(*p), false))) {
        *gp = 0;
      }  else if ((success = kvs_rw_blob(handle, TdKey(0, 0), p, sizeof(*p), false))) {
        *mp = *gp = 0;
      }
    }
    kvs_close(handle);
  }
  return success;
}

////////////////////////// transition: fs to kvs ////////////////////////////////////////////

#include "timer_data_fs.h"

int timer_data_transition_fs_to_kvs() {
  u8 result = 0;
  bool error = false;

  kvshT handle = 0;
  for (gm_iterator it; it; ++it) {
    gT g = it.getG();
    mT m = it.getM();
    Fer_TimerData td;
    if (read_timer_data_fs(&td, &g, &m, false)) {
      if (handle || (handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE))) {
        if (!kvs_rw_blob(handle,  TdKey(g, m), &td, sizeof(td), true)) {
          error = true;
          continue;
        }DB2(printf("copied timer data from fs to kvs: g=%d, m=%d\n", (int)g, (int)m));
        ++result;
      } else {
        postcond(!"kvs");
      }
    }

  }

  if (handle) {
    kvs_commit(handle);
    kvs_close(handle);
  }

  if (result && !error)
    erase_timer_data_fs(0, 0);

  return result;
}
