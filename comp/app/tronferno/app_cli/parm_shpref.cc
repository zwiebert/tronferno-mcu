

#include "fernotron/pos/shutter_prefs.h"
#include "cli_internal.hh"
#include "app_misc/opt_map.hh"
#include "app_uout/status_output.h"
#include "utils_misc/int_types.h"
#include "debug/dbg.h"
#include <string.h>
#include <utils_misc/cstring_utils.hh>
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

const char cli_help_parmShpref[] = "Store receiver related data\n\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  receiver number\n"
    "c=(store|read)"
    "mvut      duration to move from 0% (down) to 100% (up) in s/10\n"
    "mvdt      duration to move from 100% to 0% (up) in s/10\n"
    "mvspdt    duration to move from 100% to sun-position in s/10\n"
;

#define PARM_OPT_TAG "tag"
#define PARM_OPT_TAG_PREFIX PARM_OPT_TAG "."
static const char *const opts_kvd[] = {"mvut", "mvdt", "mvspdt"};
#define OPTS_KVD_SIZE (sizeof opts_kvd / sizeof opts_kvd[0])
#define OPTS_CLEAR_KEY() (p[arg_idx].key = "")

static void output_message_kvs(const struct TargetDesc &td, const char *tag, const char *val) {
  size_t key_size = strlen(tag) + sizeof PARM_OPT_TAG_PREFIX;
  char key[key_size];
  csu_copy_cat(key, key_size - 1, PARM_OPT_TAG_PREFIX, tag);
  so_arg_kvs_t arg = { .key = key, .val = val };
  soMsg_print_kvs(td, arg);
}
static void output_message_kvd(const struct TargetDesc &td, const char *key, int val) {
  so_arg_kvd_t arg = { .key = key, .val = val };
  soMsg_print_kvd(td, arg);
}

#define is_kt(k) (kt == otok::k_##k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

int process_parmShpref(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;
  int err_ct = 0;
  uint8_t g = 0, m = 0;

  bool haveStStore = false, haveStLoad = false;
  union {
    uint16_t sta[3];
    struct shutter_timings sts;
  } st = { .sta = { } };

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    assert(key);

    otok kt = optMap_findToken(key);

    if (is_kt(g)) {
      if (!asc2u8(val, &g, FER_G_MAX))
        return cli_replyFailure(td);
      OPTS_CLEAR_KEY();

    } else if (is_kt(m)) {
      if (!asc2u8(val, &m, FER_M_MAX))
        return cli_replyFailure(td);
      OPTS_CLEAR_KEY();
    }
  }

  so_object<void> shprefObj(soMsg_shpref_obj_begin,soMsg_shpref_obj_end, td);
  so_arg_gm_t shprefObjGmArgs = { .g = g, .m = m };
  so_object shprefObjGm(soMsg_shpref_obj_gm_begin, shprefObjGmArgs,soMsg_shpref_obj_gm_end, td);


  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    int k;

    if (!*key)
      continue;
    for (k = 0; k < OPTS_KVD_SIZE; ++k) {
      if (strcmp(key, opts_kvd[k]) != 0)
        continue;
      if (!haveStLoad) {
        fer_pos_prefByM_load(&st.sts, g, m);
        haveStLoad = true;
      }
      if (!is_val("?")) {
        st.sta[k] = atoi(val);
        haveStStore = true;
      }
      output_message_kvd(td, key, st.sta[k]);
      goto NEXT_ARG;
    }

    if (is_key(PARM_OPT_TAG)) {
      if (!is_val("?")) {
        goto NEXT_ARG_ERROR;
      }
      fer_shPref_strByM_forEach(td, "", g, m, output_message_kvs);

    } else if (strncmp(key, PARM_OPT_TAG_PREFIX, strlen(PARM_OPT_TAG_PREFIX)) == 0) {
      const unsigned tag_len = strlen(p[arg_idx].key) - strlen(PARM_OPT_TAG_PREFIX);
      char tag[tag_len+1];
      csu_copy((char *)tag, tag_len + 1, p[arg_idx].key + strlen(PARM_OPT_TAG_PREFIX));
      bool wildcard = false;
      if (*tag && tag[strlen(tag) - 1] == '*') {
        tag[strlen(tag) - 1] = '\0';
        wildcard = true;
      }
      if (!is_val("?")) {
        if (wildcard || !fer_shPref_strByM_store(val, tag, g, m))
          goto NEXT_ARG_ERROR;
      }
      if (wildcard)
        fer_shPref_strByM_forEach(td, tag, g, m, output_message_kvs);
      else
        fer_shPref_strByM_forOne(td, tag, g, m, output_message_kvs);

    } else {
      cli_replyFailure(td);
    }
    NEXT_ARG: continue;
    NEXT_ARG_ERROR: ++err_ct; continue;
  }

  if (haveStStore) {
    if (!fer_pos_prefByM_store(&st.sts, g, m))
      ++err_ct;
  }

  if (err_ct)
    return cli_replyFailure(td);

  return 0;
}
