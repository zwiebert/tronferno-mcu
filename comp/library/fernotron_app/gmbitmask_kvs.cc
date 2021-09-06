/*
 * shutter_state.c
 *
 *  Created on: 05.03.2019
 *      Author: bertw
 */


#ifndef MCU_ESP8266
#define CFG_NAMESPACE "Tronferno" // XXX: should rename, but don't want to lose existing data for now
#else
#define CFG_NAMESPACE "tf_gmbm"   // dedicated file, so all objects are same size
#endif

#include "app_config/proj_app_cfg.h"
#include "fernotron/pos/shutter_pct.h"
#include "debug/dbg.h"


#include <string.h>
#include "key_value_store/kvs_wrapper.h"


bool Fer_GmSet::store_load(const char *name) {
  bool success = false;

  if (kvshT handle = kvs_open(CFG_NAMESPACE, kvs_READ); handle) {
    success = (kvs_rw_blob(handle, name, (void*) mBm, 8, false));
    kvs_close(handle);
  }
  return success;
}

bool Fer_GmSet::store_save(const char *name) {
  bool success = false;

  if (kvshT handle = kvs_open(CFG_NAMESPACE, kvs_WRITE); handle) {
    success = (kvs_rw_blob(handle, name, (void*) mBm, 8, true));
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}

#include "pos/pos_map.hh"

 bool Fer_Pos_Map::store_load(const char *name, posDataT *gm) {
  bool success = false;

  if (kvshT handle = kvs_open(CFG_NAMESPACE, kvs_READ); handle) {
    success = (kvs_rw_blob(handle, name, (void*) gm, 8, false));
    kvs_close(handle);
  }
  return success;
}

 bool Fer_Pos_Map::store_save(const char *name, const posDataT *gm) {
  bool success = false;

  if (kvshT handle = kvs_open(CFG_NAMESPACE, kvs_WRITE); handle) {
    success = (kvs_rw_blob(handle, name, (void*) gm, 8, true));
    kvs_commit(handle);
    kvs_close(handle);
  }
  return success;
}
