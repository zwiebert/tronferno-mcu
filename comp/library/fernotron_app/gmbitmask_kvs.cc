/*
 * shutter_state.c
 *
 *  Created on: 05.03.2019
 *      Author: bertw
 */


#include "fernotron/pos/shutter_pct.h"
#include "debug/dbg.h"


#include <string.h>
#include "kvs/kvs_wrapper.h"


bool Fer_GmSet::store_load(const char *name) {
  bool success = false;

  if (kvshT handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ); handle) {
    success = (kvs_get_object(handle, name, mBm));
    kvs_close(handle);
  }
  return success;
}

bool Fer_GmSet::store_save(const char *name) {
  bool success = false;

  if (kvshT handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_WRITE); handle) {
    success = (kvs_set_object(handle, name, mBm));
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}

#include "pos/pos_map.hh"

 bool Fer_Pos_Map::store_load(const char *name, posDataT (&gm)[8]) {
  bool success = false;

  if (kvshT handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_READ); handle) {
    success = (kvs_get_object(handle, name, gm));
    kvs_close(handle);
  }
  return success;
}

 bool Fer_Pos_Map::store_save(const char *name, const posDataT (&gm)[8]) {
  bool success = false;

  if (kvshT handle = kvs_open(CONFIG_APP_CFG_NAMESPACE, kvs_WRITE); handle) {
    success = (kvs_set_object(handle, name, gm));
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}
