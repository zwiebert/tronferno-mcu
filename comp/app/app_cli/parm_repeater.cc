#include "app_config/proj_app_cfg.h"

#include <string.h>

#include "cli_imp.h"
#include "cli_fer.h"

#include "utils_misc/int_types.h"
#include "utils_misc/cstring_utils.hh"



#include "app_uout/status_output.h"
#include "app_misc/opt_map.hh"

#include <ctype.h>

#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

#define PRAS_TIMEOUT 15

const char cli_help_parmRepeater[] = ""
    "id-list=(?|IDs)\n"
    "id-add=ID\n"
    "id-rm=ID\n";

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, #k) == 0)
#define is_val(k) (strcmp(val, #k) == 0)

static bool id_isValid(const char *s) {
  if (strlen(s) != 6)
    return false;
  if (! (s[0] == '1' || s[0] == '2'))
    return false;

  for (int i=1; i < 6; ++i)
    if (!isxdigit(s[i]))
      return false;

  return true;
}
const int IDS_SIZE = 10;

static char ids[6* IDS_SIZE + 1];

static bool id_remove(const char *s) {
  if (!id_isValid(s))
    return false;

  for (int i = 0; i < IDS_SIZE; ++i) {
    char *t = ids + i*6;
    if (!*t)
     break;

    if (0 == strncasecmp(s, t, 6)) {
       memmove(t, t + 6, strlen(t + 6) + 1);
       return true;
    }
  }

 return false;
}

static bool id_append(const char *s) {
  if (!id_isValid(s))
    return false;
  if (strlen(ids) > (sizeof ids - 7))
    return false;

  strcat(ids, s);
  return true;
}

int process_parmRepeater(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;
  int i;
  bool print_list = false;

  so_object<void> cfgObj(&soMsg_repeater_begin, &soMsg_repeater_end, td);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL)
      return -1;

    otok kt = optMap_findToken(key);

    if (kt != otok::NONE) {
      switch (kt) {

      case otok::k_id_list: {
        if (is_val(?)) {
          print_list = true;
        } else {
        }
      }
        break;

      case otok::k_id_add: {
        if (!id_append(val))
          cli_replyFailure(td);
      }
        break;
      case otok::k_id_rm: {
        if (!id_remove(val))
          cli_replyFailure(td);

      }
        break;

      default:
        break;
      }
    } else {
      cli_replyFailure(td);
    }
  }

  if (print_list) {
    if (so_jco) {
      td.sj().add_array("id-list");
      for (int i = 0; i < IDS_SIZE; ++i) {
        const char *t = ids + i * 6;
        if (!*t)
          break;
        char buf[7];
        csu_copy(buf, 7, t, 6);
        td.sj().add_value_s(buf);
      }
      td.sj().close_array();
    }
  }

  return 0;
}
