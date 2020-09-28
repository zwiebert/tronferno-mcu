/*
 * shutter_prefs_kvs.c
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */

#include "app/config/proj_app_cfg.h"
#include "fernotron/pos/shutter_prefs.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_types.h"
#include <alloca.h>
#include <string.h>

#define SHPREF_KVS_NAMESPACE "shref"
#define ST_PREFIX "st"

bool fer_pos_prefByM_store(struct shutter_timings *src, uint8_t g, uint8_t m) {
  bool success = false;
  kvshT handle = kvs_open(SHPREF_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    char key[] = ST_PREFIX "00";
    key[sizeof(ST_PREFIX)-1] = '0' + g;
    key[sizeof(ST_PREFIX)] = '0' + m;

    success = kvs_rw_blob(handle, key, src, sizeof *src, true);
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}

bool fer_pos_prefByM_load(struct shutter_timings *dst, uint8_t g, uint8_t m) {
  bool success = false;
  kvshT handle = kvs_open(SHPREF_KVS_NAMESPACE, kvs_READ);
  if (handle) {
    char key[] = ST_PREFIX "00";
    key[sizeof(ST_PREFIX)-1] = '0' + g;
    key[sizeof(ST_PREFIX)] = '0' + m;

    success = kvs_rw_blob(handle, key, dst, sizeof *dst, false);
    kvs_close(handle);
  }
  return success;
}

#define STS_KVS_NAMESPACE "sht"
#define STS_PREFIX "ss"

#define sts_key_len(tag) (strlen(tag)+sizeof(STS_PREFIX)-1 + 2)

static char* sts_make_key(char *key, const char *tag, u8 g, u8 m) {
  char gm[] = "00";
  gm[0] += g;
  gm[1] += m;
  strcat(strcat(STRCPY(key, STS_PREFIX), gm), tag);
  return key;
}
#define sts_get_tag(key) (key+ (sizeof(STS_PREFIX)-1 + 2))

bool fer_shPref_strByM_store(const char *src, const char *tag, uint8_t g, uint8_t m) {
  bool success = false;
  kvshT handle = kvs_open(STS_KVS_NAMESPACE, kvs_WRITE);
  if (handle) {
    char *key = sts_make_key((char*)alloca(sts_key_len(tag) + 1), tag, g, m);
    success = kvs_rw_str(handle, key, (char*)src, 0, true);
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;

}
bool fer_shPref_strByM_load(char *dst, unsigned size, const char *tag, uint8_t g, uint8_t m) {

  bool success = false;
  kvshT handle = kvs_open(STS_KVS_NAMESPACE, kvs_READ);
  if (handle) {
    char *key = sts_make_key((char*)alloca(sts_key_len(tag) + 1), tag, g, m);
    success = kvs_rw_str(handle, key, dst, size, false);
    kvs_close(handle);
  }
  return success;
}

struct cb_args {
  fer_shPref_strCallBackT callback;
  const struct TargetDesc &td;
};

static kvs_cbrT fer_shPref_strByM_forEach_cb(const char *key, kvs_type_t type, void *args) {
  struct cb_args *ap = (struct cb_args*) args;
  char dst[64] = "";

  bool success = false;
  kvshT handle = kvs_open(STS_KVS_NAMESPACE, kvs_READ);
  if (handle) {
    success = kvs_rw_str(handle, key, dst, sizeof dst, false);
    kvs_close(handle);
  }
  if (success) {
    (*ap->callback)(ap->td, sts_get_tag(key), dst);
    return kvsCb_match;
  }
  return kvsCb_noMatch;
}

int fer_shPref_strByM_forEach(const struct TargetDesc &td, const char *tag, uint8_t g, uint8_t m, fer_shPref_strCallBackT callback) {
  char *key = sts_make_key((char*)alloca(sts_key_len(tag) + 1), tag, g, m);
  struct cb_args cb_args = { .callback = callback, .td = td };
  return kvs_foreach(STS_KVS_NAMESPACE, KVS_TYPE_STR, key, fer_shPref_strByM_forEach_cb, &cb_args);
}
bool fer_shPref_strByM_forOne(const struct TargetDesc &td, const char *tag, uint8_t g, uint8_t m, fer_shPref_strCallBackT callback) {
  char *key = sts_make_key((char*)alloca(sts_key_len(tag) + 1), tag, g, m);
  struct cb_args cb_args = { .callback = callback, .td = td };
  return kvsCb_match == fer_shPref_strByM_forEach_cb(key, KVS_TYPE_none, &cb_args);
}


