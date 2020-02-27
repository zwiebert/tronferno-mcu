#include "app/proj_app_cfg.h"

#include "fernotron_alias/pairings.h"
#include <stdio.h>
#include <string.h>
#include "fernotron_pos/shutter_state.h"
#include "userio_app/status_output.h"
#include "key_value_store/kvs_wrapper.h"


#ifndef DISTRIBUTION
#define D(x)
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

//#define NVS_BUG_WA

#define CFG_NAMESPACE "Tronferno"
#define CFG_PARTNAME "nvs"
#define KEY_PREFIX "cpair_"

extern char *itoa(int val, char *s, int radix);
extern char *ltoa(long val, char *s, int radix);
  

static bool read_controller(gm_bitmask_t *gm, const char *key) {
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
    gm_bitmask_t gm;
    if (!kvs_rw_blob(handle, key, &gm, sizeof gm, false)) {
      memset(gm, 0, sizeof gm);
    } else {
#ifdef NVS_BUG_WA
      success = kvs_erase_key(handle, key); // XXX: nvs-bug workaround (without nvs_erase_key an existing key can no longer be found by nvs_entry_next() after nvs_set_blob()
#endif
    }
    PUT_BIT(gm[g], m, !remove);

    bool not_empty = false;
    for (g = 0; g <= 7; ++g)
      if (gm[g] != 0) {
        not_empty = true;
        break;
      }

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
  }

  return result;
}

bool pair_rmController(uint32_t a) {
  bool success = false;
  kvshT handle;

  char key[] = "cpair_10abcd";
  ltoa(a, key+6, 16);

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
  ltoa(controller, key+6, 16);

  bool success = add_rm_controller(key, g, m, unpair);
  return success;
}

bool read_pairings(gm_bitmask_t *gm, u32 a) {
  precond(gm && (a & 0xff000000) == 0);

  char key[] = KEY_PREFIX "10abcd";
  ltoa(a, &key[sizeof(KEY_PREFIX) - 1], 16);

  bool success = read_controller(gm, key);
#ifdef NVS_BUG_WA
  if (success) {
    // there seems to be existing keys which cannot be found by iteration.
    // to fix this: erase and create them new here.
    if (!kvs_foreach(CFG_NAMESPACE, KVS_TYPE_BLOB, key, 0)) {
      kvshT handle;
      if ((handle = kvs_open(CFG_NAMESPACE, kvs_WRITE))) {
        kvs_erase_key(handle, key);
        kvs_rw_blob(handle, key, gm, sizeof(*gm), true);
        kvs_commit(handle);
        kvs_close(handle);
      }
    }
  }
#endif
  return success;
}

kvs_cbrT kvs_foreach_cb(const char *key, kvs_type_t type) {
    so_arg_kmm_t arg;
    gm_bitmask_t gm;
    arg.mm = gm;

    arg.key = &key[sizeof(KEY_PREFIX) - 1];
    read_controller(&gm, key);

    D(printf("key '%s', type '%d' a=%x \n", info.key, info.type, arg.a));
    so_output_message(SO_PAIR_PRINT_KMM, &arg);
    return kvsCb_match;
}

bool pair_so_output_all_pairings(void) {

  so_output_message(SO_PAIR_ALL_begin, NULL);

  kvs_foreach(CFG_NAMESPACE, KVS_TYPE_BLOB, KEY_PREFIX, kvs_foreach_cb);

  so_output_message(SO_PAIR_ALL_end, NULL);
  return true;
}

