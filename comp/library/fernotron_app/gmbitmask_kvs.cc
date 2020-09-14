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

#include "app/config/proj_app_cfg.h"
#include "app/settings/config.h"
#include "fernotron/pos/shutter_pct.h"
#include "debug/dbg.h"


#include <string.h>
#include "key_value_store/kvs_wrapper.h"

#define NB_SIZE 30
#define NB_PFX "GMBM_"
#define NB_N (NB_SIZE - sizeof (NB_PFX))



static bool store (const char *name, gmBitMaskT *gm, int count, bool write) {
  char nb[NB_SIZE] = NB_PFX;
  strncat (nb, name, NB_N);
  bool success = false;
  size_t len = sizeof(*gm) * count;

  kvshT handle = kvs_open(CFG_NAMESPACE, write ? kvs_WRITE : kvs_READ);

  if (handle) {
    success = (kvs_rw_blob(handle, nb, gm, len, write));
    if (write) {
      kvs_commit(handle);
    }
    kvs_close(handle);
  }
  return success;
}

bool fer_gmByName_load(const char *name, const gmBitMaskT *gm, int count) {
  return store (name, (gmBitMaskT*)gm, count, false);
}

bool fer_gmByName_store(const char *name, gmBitMaskT *gm, int count) {
  return store (name, gm, count, true);
}



