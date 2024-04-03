/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#include "timer_data_kvs.h"
#include "fernotron/fer_main.h"
#include "kvs/kvs_wrapper.h"
#include "utils_misc/int_types.h"
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
  constexpr TdKey(uint8_t g, uint8_t m) {
    myKey[2] += g;
    myKey[3] += m;
  }
  TdKey() = delete;
  constexpr operator const char*() const { return myKey; }
private:
  char myKey[5] = "td00";
};

static int delete_shadowded_kv(uint8_t group, uint8_t memb) {
  int result = 0;
  DB2(printf("delete shadowed files(group=%d, memb=%d)\n", (int)group, (int)memb));

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    for (Fer_Gm_Counter it; it; ++it) {
      const gT g = it.getG();
      const mT m = it.getM();
      if ((group == 0 || group == g) && (memb == 0 || (memb == m && fer_usedMemberMask.getMember(g, m)))) {
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

bool erase_timer_data_kvs(uint8_t g, uint8_t m) {
  precond (g <= 7 && m <= 7);
  return delete_shadowded_kv(g, m) > 0;
}

bool  save_timer_data_kvs(Fer_TimerData *p, uint8_t g, uint8_t m) {
  bool success = false;
  precond(p && g <= 7 && m <= 7);

  delete_shadowded_kv(g, m);

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    success = kvs_set_object(handle, TdKey(g, m), *p);
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;

}

bool  read_timer_data_kvs(Fer_TimerData *p, uint8_t *gp, uint8_t *mp, bool wildcard) {
  bool success = false;
  precond(p && gp && mp && *gp <= 7 && *mp <= 7);


  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_READ);
  if (handle) {
    success = kvs_get_object(handle,  TdKey(*gp, *mp), *p);

    if (!success && wildcard) {
      if ((success = kvs_get_object(handle, TdKey(*gp, 0), *p))) {
        *mp = 0;
      } else if ((success = kvs_get_object(handle, TdKey(0, *mp), *p))) {
        *gp = 0;
      }  else if ((success = kvs_get_object(handle, TdKey(0, 0), *p))) {
        *mp = *gp = 0;
      }
    }
    kvs_close(handle);
  }
  return success;
}
