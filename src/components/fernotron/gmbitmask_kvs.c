/*
 * shutter_state.c
 *
 *  Created on: 05.03.2019
 *      Author: bertw
 */

#include <string.h>
#include "key_value_store/kvs_wrapper.h"

#define CFG_NAMESPACE "Tronferno"

#include "app/proj_app_cfg.h"
#include "config/config.h"
#include "fernotron_pos/shutter_pct.h"

#define NB_SIZE 30
#define NB_PFX "GMBM_"
#define NB_N (NB_SIZE - sizeof (NB_PFX))



static int store (const char *name, gm_bitmask_t *gm, int count, bool write) {
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

int fer_gmByName_load(const char *name, const gm_bitmask_t *gm, int count) {
  return store (name, (gm_bitmask_t*)gm, count, false);
}

int fer_gmByName_store(const char *name, gm_bitmask_t *gm, int count) {
  return store (name, gm, count, true);
}

