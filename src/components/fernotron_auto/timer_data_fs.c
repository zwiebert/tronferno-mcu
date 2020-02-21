/*
 * timer_data_spiffs.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "app/proj_app_cfg.h"

#include "timer_data_fs.h"


#include "config/config.h"
#include "storage/storage.h"

#define TEST_THIS_MODULE 0

#if TEST_THIS_MODULE
#define DB(x) x
#define DB2(x)
#else
#define DB(x) do { if (C.app_verboseOutput >= vrbDebug) { x; } } while(0)
#define DB2(x) DB(x)
#endif
#define printf ets_printf



////////////////// private ///////////////////////////////////////////////////////////////

static char * 
gm_to_file_name (u8 g, u8 m) {
  static char file_name[] = "tdxx.bin";
  if (g > 7 || m > 7) {
    return NULL;
  }

  file_name[2] = '0'+g;
  file_name[3] = '0'+m;

  return file_name;
}

static bool  save_data2(timer_data_t *p, const char *file_name) {
  return write_to_file(file_name, p, sizeof (timer_data_t));
}


static bool  read_data2(timer_data_t *p, const char *file_name) {
  return read_from_file(file_name, p, sizeof (timer_data_t));
}

static int  delete_shadowded_files(u8 group, u8 memb) {
  int g, m, result = 0;
  DB2(printf("delete shadowed files(group=%d, memb=%d)\n", (int)group, (int)memb));
  for (g = 0; g <= 7; ++g) {
    for (m = 0; m <= 7; ++m) {
      if ((group == 0 || group == g) && (memb == 0 || memb == m)) {
        if (delete_file(gm_to_file_name(g, m))) {
          DB2(printf("shadow deleted: g=%d, m=%d, fid=%s\n", (int)g, (int)m, gm_to_file_name(g, m)));
          ++result;
        }
      }
    }
  }
  return result;
}

////////////////////////////////// public ////////////////////////////////////////////////////////////////////


bool  save_timer_data_fs(timer_data_t *p, u8 g, u8 m) {
  bool result = false;

  delete_shadowded_files(g, m);
  result = save_data2(p, gm_to_file_name(g, m));

  return result;
}

bool  read_timer_data_fs(timer_data_t *p, u8 *g, u8 *m, bool wildcard) {
  bool result;

#if 0 // FIXME: let it crash, to find programming errors?
  if (g > 7 || m > 7) {
    return false;
  }
#endif

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

