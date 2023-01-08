

#include <string.h>
#include "fernotron/fer_main.h"
#include "fernotron/pos/shutter_pct.h"
#include "cli_imp.h"
#include "cli_fer.h"
#include "utils_misc/int_types.h"
#include "fernotron/alias/pairings.h"
#include "fernotron/sep/set_endpos.h"
#include "app_uout/status_output.h"
#include "app_misc/opt_map.hh"

#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

const char cli_help_parmSep[] = "'sep' sets roller shutter end positions\n\n"
    "a=(?|ID)  0  transmitter-ID or receiver radio code\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  receiver number\n"
    "c=(down|keep-moving|stop|test)\n"
    "request-auth=(button)\n"
    "auth-key=N\n"
    "enable=(on|off)\n"
;

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

typedef bool (*move_funT)(uint32_t auth_key);

int process_parmSep(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;
  bool enable = false;
  bool request_auth = false;
  bool request_unauth = false;
  uint32_t auth_key = 0;
  move_funT move_fun = 0;

  so_object<void> cfgObj(&soMsg_sep_obj_begin, &soMsg_sep_obj_end, td);

  uint32_t addr = fer_config.cu;
  uint8_t g = 0, m = 0;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL)
      return -1;

    otok kt = optMap_findToken(key);

    if (kt != otok::NONE) {
      switch (kt) {

      case otok::k_request_auth: {
        if (is_val("button")) {
        request_auth = true;
        } else if (is_val("off")) {
          request_unauth = true;
        }
      }
        break;

      case otok::k_auth_key: {
        auth_key = strtoul(val, 0, 10);
      }
        break;

      case otok::k_enable: {
        if (is_val("off")) {
          fer_sep_disable();
        } else if (is_val("on")) {
          enable = true;
        }
      }
        break;

      case otok::k_a: {
        uint32_t tmp = val ? strtol(val, NULL, 16) : 0;
        if (tmp) {
          addr = tmp;
          if (*val == '0')
            addr += 0x900000; // convert radio-code to real address
        } else if (!is_val("0"))
          return cli_replyFailure(td);
      }
        break;


      case otok::k_g: {
        if (!asc2u8(val, &g, 7))
          return cli_replyFailure(td);
      }
        break;
      case otok::k_m: {
        if (!asc2u8(val, &m, 7))
          return cli_replyFailure(td);
      }
        break;
      case otok::k_c: {
        if (is_val("up")) {
          move_fun = fer_sep_move_up;
        } else if (is_val("down")) {
          move_fun = fer_sep_move_down;
        } else if (is_val("keep-moving")) {
          move_fun = fer_sep_move_continue;
        } else if (is_val("stop")) {
          move_fun = fer_sep_move_stop;
        }else if (is_val("test")) {
          fer_sep_move_test();
        }
      }
        break;

      default:
        break;
      }
    } else {
      cli_replyFailure(td);
    }
  }

  if (request_unauth) {
    fer_sep_deauthenticate(td, auth_key);
  }

  if (request_auth) {
    if (!fer_sep_authenticate(td, auth_key)) {
      cli_replyFailure(td);
      td.so().print("auth-terminated", 1);
      return -1;
    }

  }

  if (enable) {
    fer_sep_enable(td, auth_key, addr, g, m);
  }

  if (move_fun) {
    (*move_fun)(auth_key);
  }

  return 0;
}
