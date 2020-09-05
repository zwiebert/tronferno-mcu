/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "timer_data_fs.h"
#include "config/config.h"
#include "storage/storage.h"
#include "misc/int_types.h"
#include "debug/dbg.h"
#include "fernotron/types.h"

#ifndef TEST_HOST
#define printf ets_printf
#define DB(x) do { if (TXTIO_IS_VERBOSE(vrbDebug)) { x; } } while(0)
#define DB2(x) DB(x)
#else
#include <stdio.h>
#define DB(x) x
#define DB2(x)
#endif


////////////////// private ///////////////////////////////////////////////////////////////

static char * 
gm_to_file_name (u8 g, u8 m) {
  precond (g <= 7 && m <= 7);
  static char file_name[] = "tdxx.bin";

  file_name[2] = '0'+g;
  file_name[3] = '0'+m;

  return file_name;
}

static bool  save_data2(timer_data_t *p, const char *file_name) {
  return stor_fileWrite(file_name, p, sizeof (timer_data_t));
}


static bool  read_data2(timer_data_t *p, const char *file_name) {
  return stor_fileRead(file_name, p, sizeof (timer_data_t));
}

static int delete_shadowded_files(u8 group, u8 memb) {
  int g, m, result = 0;
  DB2(printf("delete shadowed files(group=%d, memb=%d)\n", (int)group, (int)memb));
  for (gm_iterator it; it; ++it) {
    const gT g = it.getG();
    const mT m = it.getM();
    if ((group == 0 || group == g) && (memb == 0 || (memb == m && C.fer_usedMemberMask.getBit(g, m)))) {
      if (stor_fileDelete(gm_to_file_name(g, m))) {
        DB2(printf("shadow deleted: g=%d, m=%d, fid=%s\n", (int)g, (int)m, gm_to_file_name(g, m)));
        ++result;
      }
    }
  }

  return result;
}


////////////////////////////////// public ////////////////////////////////////////////////////////////////////

bool erase_timer_data_fs(u8 g, u8 m) {
  precond (g <= 7 && m <= 7);
  return delete_shadowded_files(g, m) > 0;
}

bool  save_timer_data_fs(timer_data_t *p, u8 g, u8 m) {
  bool result = false;
  precond(p && g <= 7 && m <= 7);

  delete_shadowded_files(g, m);
  result = save_data2(p, gm_to_file_name(g, m));

  return result;
}

bool  read_timer_data_fs(timer_data_t *p, u8 *g, u8 *m, bool wildcard) {
  bool result;
  precond(p && g && m && *g <= 7 && *m <= 7);

  result = read_data2(p, gm_to_file_name(*g, *m));

  if (!result && wildcard) {
    if ((result = read_data2(p, gm_to_file_name(*g, 0)))) {
      *m = 0;
    } else if ((result = read_data2(p, gm_to_file_name(0, *m)))) {
      *g = 0;
    } else if ((result = read_data2(p, gm_to_file_name(0, 0)))) {
      *g = *m = 0;
    }
  }
  return result;
}

