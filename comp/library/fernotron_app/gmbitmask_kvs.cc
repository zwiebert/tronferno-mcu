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

#define NB_SIZE 30
#define NB_PFX "GMBM_"
#define NB_N (NB_SIZE - sizeof (NB_PFX))



static bool gm_save(const char *name, const gmSetT gm[], int count, bool write) {
  char nb[NB_SIZE] = NB_PFX;
  strncat (nb, name, NB_N);
  bool success = false;
  const size_t byte_len = sizeof(gm[0]) * count;

  kvshT handle = kvs_open(CFG_NAMESPACE, write ? kvs_WRITE : kvs_READ);

  if (handle) {
    success = (kvs_rw_blob(handle, nb, (void*)gm, byte_len, write));
    if (write) {
      kvs_commit(handle);
    }
    kvs_close(handle);
  }
  return success;
}

bool fer_stor_gmSet_load(const char *name, gmSetT gm[], int count) {
  return gm_save (name, (gmSetT*)gm, count, false);
}

bool fer_stor_gmSet_save(const char *name, const gmSetT gm[], int count) {
  return gm_save (name, gm, count, true);
}



