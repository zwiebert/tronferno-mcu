

#include "fernotron/alias/pairings.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include "app_uout/status_output.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "key_value_store/kvs_wrapper.h"
#include "utils_misc/int_types.h"
#include "utils_misc/cstring_utils.h"
#include "utils_misc/itoa.h"
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

#define CONFIG_APP_CFG_NAMESPACE "Tronferno"
#define CFG_PARTNAME "nvs"
#define CPAIR_KEY_PREFIX "cpair_"
#define CPAIR_KEY_PREFIX_LEN (sizeof CPAIR_KEY_PREFIX - 1)
#define CPAIR_KEY_LEN (CPAIR_KEY_PREFIX_LEN + 6)



static void  fixController(const char *key, Fer_GmSet *gm) {
  // there seems to be existing keys which cannot be found by iteration.
  // to fix this: erase and create them new here.
  if (!kvs_foreach(CONFIG_APP_CFG_NAMESPACE, KVS_TYPE_BLOB, csu_areEqual, key)) {
    kvshT handle;
    if ((handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_WRITE))) {
      kvs_erase_key(handle, key);
      kvs_set_object(handle, key, *gm);
      kvs_commit(handle);
      kvs_close(handle);
    }
  }
}


static bool read_controller(Fer_GmSet *gm, const char *key) {
  bool success = false;
  kvshT handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ);
  if (handle) {
    success = kvs_get_object(handle, key, *gm);
    kvs_close(handle);
  }
  return success;
}

static bool
add_rm_controller(const char *key, Fer_GmSet mm, bool remove) {
  bool success;
  kvshT handle;
  bool result = false;

  handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ_WRITE);
  if (handle) {
    Fer_GmSet gm;
    if (!kvs_get_object(handle, key, gm)) {
      gm.clear();
    }

    if (remove)
      gm &= ~mm;
    else
      gm |= mm;

    bool not_empty = !gm.isAllClear();

    if (not_empty) {
      success = kvs_set_object(handle, key, gm);
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

#define a2key(a)     char key[] = CPAIR_KEY_PREFIX "10abcd"; itoa(a, &key[sizeof(CPAIR_KEY_PREFIX) - 1], 16);


bool fer_alias_setControllerPairings(uint32_t controller, Fer_GmSet *mm) {
  bool success = false;
  a2key(controller);

  kvshT handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_WRITE);
  if (handle) {
    if (mm->isAllClear()) {
      success = kvs_erase_key(handle, key) && kvs_commit(handle);
    } else {
      success = kvs_set_object(handle, key, *mm) && kvs_commit(handle);
    }
    kvs_close(handle);
#ifdef NVS_BUG_WA
    if (success) {
      fixController(key, mm);
    }
#endif
  }

  return success;
}

bool fer_alias_rmController(uint32_t a) {
  bool success = false;
  kvshT handle;

  a2key(a);

  handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ_WRITE);
  if (handle) {
    success = kvs_erase_key(handle, key) && kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}


bool fer_alias_controller(uint32_t controller, uint8_t g, uint8_t m, bool unpair) {
  D(ets_printf("%s: g=%d, m=%d, unpair=%d\n", __func__, (int)g, (int)m, (int)unpair));
  precond((controller & 0xff000000) == 0);
  precond(g <= 7 && m <= 7);

  char key[] = "cpair_10abcd";
  itoa(controller, key + 6, 16);

  Fer_GmSet gm { };
  if (g && m) {
    gm.setMember(g, m);
  } else if (g == 0) {
    for (int gi = 1; gi <= 7; ++gi) {
      for (int mi = 1; mi <= 7; ++mi) {
        gm.setMember(gi, mi);
      }
    }
  } else {
    for (int mi = 1; mi <= 7; ++mi) {
      gm.setMember(g, mi);
    }
  }
  bool success = add_rm_controller(key, gm, unpair);
  return success;
}


bool fer_alias_getControllerPairings(uint32_t a, Fer_GmSet *gm) {
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

static bool cpairKey_isValid(const char *key) {
  if (strlen(key) != CPAIR_KEY_LEN)
    return false;

  if (strncmp(key, CPAIR_KEY_PREFIX, CPAIR_KEY_PREFIX_LEN) != 0)
    return false;

  key += CPAIR_KEY_PREFIX_LEN;
  for (int i=0; i < 6; ++i) {
    if (!isxdigit(key[i]))
      return false;
  }
  return true;
}

static void remove_key(const char *key) {
  if (kvshT h = kvs_open(key, kvs_WRITE)) {
    kvs_erase_key(h, key);
    kvs_commit(h);
    kvs_close(h);
  }
}

static kvs_cbrT kvs_foreach_cb(const char *key, kvs_type_t type, const UoutWriter &td) {
  if (!cpairKey_isValid(key)) {
    remove_key(key);  //delete this invalid key from kvs
    D(ets_printf("%s: key=<%s>, type=%d\n", __func__, key, (int)type));
    return kvsCb_noMatch;
  }

    Fer_GmSet gm {};
    so_arg_kmm_t arg { .key =  key + CPAIR_KEY_PREFIX_LEN, .mm = &gm };

    if (!read_controller(&gm, key))
      return kvsCb_noMatch;

    if (gm.isAllClear()) {
      remove_key(key);  //delete this empty value from kvs
      return kvsCb_noMatch;
    }

    D(printf("key '%s', type '%d' a=%x \n", info.key, info.type, arg.a));
    soMsg_pair_print_kmm(td, arg);
    return kvsCb_match;
}

bool fer_alias_so_output_all_pairings(const class UoutWriter &td, bool content_only) {

  if (!content_only)
    soMsg_pair_all_begin(td);

  kvs_foreach(CONFIG_APP_CFG_NAMESPACE, KVS_TYPE_BLOB, csu_startsWith, CPAIR_KEY_PREFIX, kvs_foreach_cb, td);

  if (!content_only)
    soMsg_pair_all_end(td);
  return true;
}

