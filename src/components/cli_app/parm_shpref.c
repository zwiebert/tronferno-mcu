
#include "app/proj_app_cfg.h"
#include "fernotron_pos/shutter_prefs.h"
#include "cli_imp.h"
#include "userio_app/status_output.h"

#include <string.h>

#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

#define FER_G_MAX 7
#define FER_M_MAX 7

#define PRAS_TIMEOUT 15

const char help_parmShpref[] = ""
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
;

int process_parmShpref(clpar p[], int len) {
  int arg_idx;

  so_output_message(SO_SHPREF_begin, NULL);

  u32 addr = 0;
  u8 g = 0, m = 0, c = 0;

  bool store = false, read = false;

  struct shutter_timings st = { 0, };

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;

    } else if (strcmp(key, "g") == 0) {
      if (!asc2u8(val, &g, FER_G_MAX))
        return reply_failure();

    } else if (strcmp(key, "m") == 0) {
      if (!asc2u8(val, &m, FER_M_MAX))
        return reply_failure();

    } else if (strcmp(key, "mvut") == 0) {
      st.move_up_tsecs = atoi(val);

    } else if (strcmp(key, "mvdt") == 0) {
      st.move_down_tsecs = atoi(val);

    } else if (strcmp(key, "c") == 0) {
      if (strcmp(val, "store") == 0) {
        store = true;
      } else if (strcmp(val, "read") == 0) {
        read = true;
      }

    } else {
      reply_failure();
    }
  }

  if (store) {
    if(shuPref_save_timings(&st, g, m))
      read = true;
  }

  if (read) {
    if (shuPref_read_timings(&st, g, m)) {
      so_arg_gmt_t arg;
      arg.g = g;
      arg.m = m;
      arg.st = &st;
      so_output_message(SO_SHPREF_PRINT_GMT, &arg);
    }


  }

  so_output_message(SO_SHPREF_end, NULL);
  return 0;
}
