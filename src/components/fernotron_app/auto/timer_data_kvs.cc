/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "timer_data_kvs.h"
#include "config/config.h"
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
#define TD_KVS_NAMESPACE "auto_td"
#define TD_PREFIX "td"

#define td_key_len(tag) (strlen(tag)+sizeof(TD_PREFIX)-1 + 2)

static char* td_make_key(char *key, const char *tag, u8 g, u8 m) {
  char gm[] = "00";
  gm[0] += g;
  gm[1] += m;
  strcat(strcat(strcpy(key, TD_PREFIX), gm), tag);
  return key;
}
#define td_get_tag(key) (key+ (sizeof(TD_PREFIX)-1 + 2))

#define tag ""

static int delete_shadowded_kv(u8 group, u8 memb) {
  int result = 0;
  DB2(printf("delete shadowed files(group=%d, memb=%d)\n", (int)group, (int)memb));

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    char key[td_key_len(tag) + 1];
    for (gm_iterator it; it; ++it) {
      const gT g = it.getG();
      const mT m = it.getM();
      if ((group == 0 || group == g) && (memb == 0 || (memb == m && C.fer_usedMemberMask.getBit(g, m)))) {
        if (kvs_erase_key(handle, td_make_key(key, tag, group, memb))) {
          DB2(printf("shadow deleted: g=%d, m=%d, key=%s\n", (int)g, (int)m, key));
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

bool  save_timer_data_kvs(timer_data_t *p, u8 g, u8 m) {
  bool success = false;
  precond(p && g <= 7 && m <= 7);

  delete_shadowded_kv(g, m);

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    char *key = td_make_key((char*)alloca(td_key_len(tag) + 1), tag, g, m);
    success = kvs_rw_blob(handle, key, p, sizeof(*p), true);
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;

}

bool  read_timer_data_kvs(timer_data_t *p, u8 *g, u8 *m, bool wildcard) {
  bool success = false;
  precond(p && g && m && *g <= 7 && *m <= 7);

  kvshT handle = kvs_open(TD_KVS_NAMESPACE, kvs_READ);
  if (handle) {
    char *key = td_make_key((char*)alloca(td_key_len(tag) + 1), tag, *g, *m);
    success = kvs_rw_blob(handle, key, p, sizeof(*p), false);

    if (!success && wildcard) {
      if ((success = kvs_rw_blob(handle, td_make_key(key, tag, *g, 0), p, sizeof(*p), false))) {
        *m = 0;
      } else if ((success = kvs_rw_blob(handle, td_make_key(key, tag, 0, *m), p, sizeof(*p), false))) {
        *g = 0;
      }  else if ((success = kvs_rw_blob(handle, td_make_key(key, tag, 0, *m), p, sizeof(*p), false))) {
        *m = *g = 0;
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
    timer_data_t td;
    if (read_timer_data_fs(&td, &g, &m, false)) {
      if (handle || (handle = kvs_open(TD_KVS_NAMESPACE, kvs_WRITE))) {
        char *key = td_make_key((char*) alloca(td_key_len(tag) + 1), tag, g, m);
        if (!kvs_rw_blob(handle, key, &td, sizeof(td), true)) {
          error = true;
          continue;
        }DB2(printf("copied timer data from fs to kvs: g=%d, m=%d, key=%s\n", (int)g, (int)m, key));
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