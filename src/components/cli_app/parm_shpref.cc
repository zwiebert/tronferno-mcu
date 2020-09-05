
#include "app_config/proj_app_cfg.h"
#include "fernotron_pos/shutter_prefs.h"
#include "cli_app/cli_imp.h"
#include "userio_app/status_output.h"
#include "misc/int_types.h"
#include "debug/dbg.h"
#include <string.h>
#include <alloca.h>
#include <stdlib.h>

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

const char cli_help_parmShpref[] = ""
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "c=(store|read)"
    "mvut      duration to move from 0% (down) to 100% (up) in s/10\n"
    "mvdt      duration to move from 100% to 0% (up) in s/10\n"
    "mvspdt    duration to move from 100% to sun-position in s/10\n"
;

#define PARM_OPT_TAG "tag"
#define PARM_OPT_TAG_PREFIX PARM_OPT_TAG "."
static const char *const opts_kvd[] = {"mvut", "mvdt", "mvspdt"};
#define OPTS_KVD_SIZE (sizeof opts_kvd / sizeof opts_kvd[0])
#define OPTS_CLEAR_KEY() (*p[arg_idx].key = '\0')

static void output_message_begin(u8 g, u8 m) {
  so_arg_gm_t gm = { .g = g, .m = m };
  so_output_message(SO_SHPREF_OBJ_GM_begin, &gm);
}
static void output_message_kvs(const char *tag, const char *val) {
  char *key = strcat(strcpy((char*)alloca(strlen(tag) + sizeof PARM_OPT_TAG_PREFIX), PARM_OPT_TAG_PREFIX), tag);
  so_arg_kvs_t arg = { .key = key, .val = val };
  so_output_message(SO_PRINT_KVS, &arg);
}
static void output_message_kvd(const char *key, int val) {
  so_arg_kvd_t arg = { .key = key, .val = val };
  so_output_message(SO_PRINT_KVD, &arg);
}

int process_parmShpref(clpar p[], int len) {
  int arg_idx;
  int err_ct = 0;
  u8 g = 0, m = 0;

  bool haveStStore = false, haveStLoad = false;
  union {
    u16 sta[3];
    struct shutter_timings sts;
  } st = { .sta = { } };

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    assert(key);

    if (strcmp(key, "g") == 0) {
      if (!asc2u8(val, &g, FER_G_MAX))
        return cli_replyFailure();
      OPTS_CLEAR_KEY();

    } else if (strcmp(key, "m") == 0) {
      if (!asc2u8(val, &m, FER_M_MAX))
        return cli_replyFailure();
      OPTS_CLEAR_KEY();
    }
  }

  so_output_message(SO_SHPREF_OBJ_begin, 0);
  output_message_begin(g, m);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    int k;
    if (!*key)
      continue;
    for (k = 0; k < OPTS_KVD_SIZE; ++k) {
      if (strcmp(key, opts_kvd[k]) != 0)
        continue;
      if (!haveStLoad) {
        ferPos_prefByM_load(&st.sts, g, m);
        haveStLoad = true;
      }
      if (*val != '?') {
        st.sta[k] = atoi(val);
        haveStStore = true;
      }
      output_message_kvd(key, st.sta[k]);
      goto NEXT_ARG;
    }

    if (strcmp(key, PARM_OPT_TAG) == 0) {
      if (strcmp(val, "?") != 0) {
        goto NEXT_ARG_ERROR;
      }
      ferSp_strByM_forEach("", g, m, output_message_kvs);

    } else if (strncmp(key, PARM_OPT_TAG_PREFIX, strlen(PARM_OPT_TAG_PREFIX)) == 0) {
      char *tag = p[arg_idx].key + strlen(PARM_OPT_TAG_PREFIX);
      bool wildcard = false;
      if (*tag && tag[strlen(tag) - 1] == '*') {
        tag[strlen(tag) - 1] = '\0';
        wildcard = true;
      }
      if (strcmp(val, "?") != 0) {
        if (wildcard || !ferSp_strByM_store(val, tag, g, m))
          goto NEXT_ARG_ERROR;
      }
      if (wildcard)
        ferSp_strByM_forEach(tag, g, m, output_message_kvs);
      else
        ferSp_strByM_forOne(tag, g, m, output_message_kvs);

    } else {
      cli_replyFailure();
    }
    NEXT_ARG: continue;
    NEXT_ARG_ERROR: ++err_ct; continue;
  }

  so_output_message(SO_SHPREF_OBJ_GM_end, NULL);
  so_output_message(SO_SHPREF_OBJ_end, NULL);

  if (haveStStore) {
    if (!ferPos_prefByM_store(&st.sts, g, m))
      ++err_ct;
  }

  if (err_ct)
    return cli_replyFailure();

  return 0;
}
