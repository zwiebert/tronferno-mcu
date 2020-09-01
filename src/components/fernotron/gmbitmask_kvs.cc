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
#include "config/config.h"
#include "fernotron_pos/shutter_pct.h"
#include "debug/debug.h"


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

#ifndef gm_isAllClear
bool gm_isAllClear(gm_bitmask_t *gm) {
  int i;
  for (i = 0; i < 8; ++i)
    if (gm_GetByte(gm, i))
      return false;
  return true;
}

int gm_countSetBits(gm_bitmask_t *gm) {
  u8 g, m;
  int remaining = 0;

  for (g = 1; g < 8; ++g) {
    for (m = 1; m < 8; ++m) {
      if (gm_GetBit(gm, g, m)) {
        ++remaining;
      }
    }
  }
  return remaining;
}
void gm_fromNibbleCounters(gm_bitmask_t *gm, u32 um) {
  int g, m;
  gm_Clear(gm);
  for (g=0; g < 8; ++g, (um >>=4)) {
    u8 u = um & 0x07;
    for (m=1; m <= u; ++m) {
      gm_SetBit(gm,g,m);
    }
  }
}


bool gm_getNext(gm_bitmask_t *gm, u8 *gp, u8 *mp) {
  precond(gm && gp && mp);
  precond(*gp <= 7 && (*mp <= 7 || *mp == 0xff));

  u8 g = *gp, m = *mp;

  while (true) {
    if (++m > 7) {
      if (++g > 7) {
        break;
      } else {
        m = -1;
        continue;
      }
    }

    postcond(g < 8 && m < 8);

    if (!gm_GetByte(gm, g)) {
      m = 7;
      continue;
    }

    if (gm_GetBit(gm, g, m)) {
      *gp = g;
      *mp = m;
      return true;
    }
  }
  return false;
}

#endif

