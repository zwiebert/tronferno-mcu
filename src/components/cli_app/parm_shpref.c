
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
    "mvut      duration to move from 0% (down) to 100% (up) in s/10\n"
    "mvdt      duration to move from 100% to 0% (up) in s/10\n"
    "mvspdt    duration to move from 100% to sun-position in s/10\n"
;

int process_parmShpref(clpar p[], int len) {
  int arg_idx;

  so_output_message(SO_SHPREF_begin, NULL);

  u8 g = 0, m = 0;

  bool store = false, read = false;
  bool haveMvut = false, haveMvdt = false, haveMvspdt = false;

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
      haveMvut = true;

    } else if (strcmp(key, "mvdt") == 0) {
      st.move_down_tsecs = atoi(val);
      haveMvdt = true;

    } else if (strcmp(key, "mvspdt") == 0) {
      st.move_sundown_tsecs = atoi(val);
      haveMvspdt = true;

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
    struct shutter_timings rt = {0,};
    if (ferPos_prefByM_load(&rt, g, m)) {
      if (!haveMvut)
        st.move_up_tsecs = rt.move_up_tsecs;
      if (!haveMvdt)
        st.move_down_tsecs = rt.move_down_tsecs;
      if (!haveMvspdt)
        st.move_sundown_tsecs = rt.move_sundown_tsecs;
    }
    if(ferPos_prefByM_store(&st, g, m))
      read = true;
  }

  if (read) {
    if (ferPos_prefByM_load(&st, g, m)) {
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
