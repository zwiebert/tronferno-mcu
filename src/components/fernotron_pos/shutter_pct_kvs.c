/*
 * shutter_state.c
 *
 *  Created on: 05.03.2019
 *      Author: bertw
 */

#include <stdio.h>
#include <string.h>
#include "key_value_store/kvs_wrapper.h"

#define CFG_NAMESPACE "Tronferno"

#include "app/proj_app_cfg.h"
#include "config/config.h"
#include "fernotron_pos/shutter_pct.h"

#define NB_SIZE 30
#define NB_PFX "GMBM_"
#define NB_N (NB_SIZE - sizeof (NB_PFX))

typedef u8 u8a8[8];

static int store (const char *name, u8a8 *gm, int count, bool write) {
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

int ferPos_gmByName_load(const char *name, const gm_bitmask_t gm, int count) {
  return store (name, (u8a8 *)gm, count, false);
}

int ferPos_gmByName_store(const char *name, gm_bitmask_t gm, int count) {
  return store (name, (u8a8 *)gm, count, true);
}


static char *g_to_name(u8 g, char *buf) {
  strcpy(buf, "PMAP_Gx");
  buf[6] = '0' + g;
  return buf;
}

 int ferPos_pctsByGroup_load(u8 g, const shutterGroupPositionsT positions) {
   char buf[8];
  return store (g_to_name(g, buf), (u8a8 *)positions, 1, false);
}

int ferPoas_pctsByGroup_store(u8 g, shutterGroupPositionsT positions) {
  char buf[8];
  return store (g_to_name(g, buf), (u8a8 *)positions, 1, true);
}
