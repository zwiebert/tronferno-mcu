#include "app/config/proj_app_cfg.h"

#include "fernotron/alias/pairings.h"
#include <stdio.h>
#include <string.h>
#include "app/uout/status_output.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"
#include "misc/itoa.h"
#include "debug/dbg.h"

#ifndef DISTRIBUTION
#define D(x)
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

#define NVS_BUG_WA

#define CFG_NAMESPACE "Tronferno"
#define CFG_PARTNAME "nvs"
#define KEY_PREFIX "cpair_"


  
static void  fixController(const char *key, GmBitMask *gm) {
  // there seems to be existing keys which cannot be found by iteration.
  // to fix this: erase and create them new here.
  if (!kvs_foreach(CFG_NAMESPACE, KVS_TYPE_BLOB, key, 0, 0)) {
    kvshT handle;
    if ((handle = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
      kvs_erase_key(handle, key);
      kvs_rw_blob(handle, key, gm, sizeof(*gm), true);
      kvs_commit(handle);
      kvs_close(handle);
    }
  }
}


static bool read_controller(GmBitMask *gm, const char *key) {
  bool success = false;
  kvshT handle = kvs_open(CFG_NAMESPACE, kvs_READ);
  if (handle) {
    success = kvs_rw_blob(handle, key, gm, sizeof *gm, false);
    kvs_close(handle);
  }
  return success;
}

static bool
add_rm_controller(const char *key, u8 g, u8 m, bool remove) {
  bool success;
  kvshT handle;
  bool result = false;

  D(ets_printf("%s: key=\"%s\", g=%d, m=%d, remove=%d\n", __func__, key, (int)g, (int)m, (int)remove));
  precond(key && 1 <= g && g <= 7 && 1 <= m && m <= 7);

  handle = kvs_open(CFG_NAMESPACE, kvs_READ_WRITE);
  if (handle) {
    GmBitMask gm;
    if (!kvs_rw_blob(handle, key, &gm, sizeof gm, false)) {
      gm.clear();
    }

    gm.putBit(g, m, !remove);

    bool not_empty = !gm.isAllClear();

    if (not_empty) {
      success = kvs_rw_blob(handle, key, &gm, sizeof gm, true);
    } else {
      success = kvs_erase_key(handle, key);
    }

    if (success) {
      result = true;
      kvs_commit(handle);
    }

    kvs_close(handle);
#ifdef NVS_BUG_WA
    if (not_empty && result) {
      fixController(key, &gm);
    }
#endif
  }


  return result;
}

#define a2key(a)     char key[] = KEY_PREFIX "10abcd"; itoa(a, &key[sizeof(KEY_PREFIX) - 1], 16);




bool pair_setControllerPairings(uint32_t controller, GmBitMask *mm) {
  bool success = false;
  a2key(controller);

  kvshT handle = kvs_open(CFG_NAMESPACE, kvs_WRITE);
  if (handle) {
    success = kvs_rw_blob(handle, key, mm, sizeof *mm, true) && kvs_commit(handle);
    kvs_close(handle);
#ifdef NVS_BUG_WA
    if (success) {
      fixController(key, mm);
    }
#endif
  }

  return success;
}

bool pair_rmController(uint32_t a) {
  bool success = false;
  kvshT handle;

  a2key(a);

  handle = kvs_open(CFG_NAMESPACE, kvs_READ_WRITE);
  if (handle) {
    success = kvs_erase_key(handle, key) && kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}

bool
pair_controller(u32 controller, u8 g, u8 m, bool unpair) {
  D(ets_printf("%s: g=%d, m=%d, unpair=%d\n", __func__, (int)g, (int)m, (int)unpair));
  precond ((controller & 0xff000000) == 0);
  precond (1 <= g && g <= 7 && 1 <= m && m <= 7);

  char key[] = "cpair_10abcd";
  itoa(controller, key+6, 16);

  bool success = add_rm_controller(key, g, m, unpair);
  return success;
}


bool pair_getControllerPairings(u32 a, GmBitMask *gm) {
  precond(gm && (a & 0xff000000) == 0);

  a2key(a);


  bool success = read_controller(gm, key);
#ifdef NVS_BUG_WA
  if (success) {
    fixController(key, gm);
  }
#endif
  return success;
}

kvs_cbrT kvs_foreach_cb(const char *key, kvs_type_t type, void *args) {
    so_arg_kmm_t arg;
    GmBitMask gm;
    arg.mm = &gm;

    arg.key = &key[sizeof(KEY_PREFIX) - 1];
    read_controller(&gm, key);

    D(printf("key '%s', type '%d' a=%x \n", info.key, info.type, arg.a));
    soMsg_pair_print_kmm(arg);
    return kvsCb_match;
}

bool pair_so_output_all_pairings(void) {

  soMsg_pair_all_begin();

  kvs_foreach(CFG_NAMESPACE, KVS_TYPE_BLOB, KEY_PREFIX, kvs_foreach_cb, 0);

  soMsg_pair_all_end();
  return true;
}

