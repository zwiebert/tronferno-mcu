/*
 * shutter_prefs_kvs.c
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */


#include "shutter_prefs.h"
#include "key_value_store/kvs_wrapper.h"

#define SHPREF_KVS_NAMESPACE "shref"
#define ST_PREFIX "st"

bool shuPref_save_timings(struct shutter_timings *src, uint8_t g, uint8_t m) {
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

bool shuPref_read_timings(struct shutter_timings *dst, uint8_t g, uint8_t m) {
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
