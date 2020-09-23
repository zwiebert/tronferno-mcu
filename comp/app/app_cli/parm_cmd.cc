#include "app/config/proj_app_cfg.h"

#include <string.h>
#include <fernotron/fer_msg_rx.h>
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/commands.h"
#include "fernotron/pos/shutter_pct.h"
#include "txtio/inout.h"
#include "app/config/proj_app_cfg.h"
#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_msg_attachment.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "app/uout/status_output.h"
#include "app/settings/config.h"
#include "app/cli/cli_fer.h"
#include "app/opt_map.hh"
#include <stdlib.h>


#define FER_SB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

const char cli_help_parmSend[]  =
    "a=(0|ID)  0  hex ID of sender or receiver.\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "c=(up|down|stop|sun-down|sun-inst|set|?)\n"
    "p=?          get current position\n"
    "r=N       2  repeat command 1+N times\n"
    "SEP[=0|1]    Enter end-position adjustment mode (needs hardware button)"
// "TROT        Toggle rotation direction"
;

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

int 
process_parmSend(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;

  u32 addr = cfg_getCuId();
  u8 g = 0, m = 0;
  fer_cmd cmd = fer_cmd_None;
  int set_end_pos = -1;
  u8 repeats = FER_SB_PLAIN_REPEATS;
  bool has_requested_position = false;
  i8 pct = -1;
#define has_pct (pct >= 0)
#define has_cmd (cmd != fer_cmd_None)
#define has_sep (set_end_pos >= 0)


  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (!key)
      return -1;

    otok kt = optMap_findToken(key);

    if (kt != otok::NONE) {
      switch (kt) {
      case otok::a: {
        u32 tmp = val ? strtol(val, NULL, 16) : 0;
        if (tmp)
          addr = tmp;
      }
        break;

      case otok::g: {
        int arg = atoi(val);
        if (0 <= arg && arg <= 7) {
          g = arg;
        } else {
          return cli_replyFailure(td);
        }
      }
        break;

      case otok::m: {
        int arg = atoi(val);
        if (0 <= arg && arg <= 7) {
          m = arg;
        } else {
          return cli_replyFailure(td);
        }
      }
        break;

      case otok::r: {
        NODEFAULT();
        repeats = atoi(val);
        if (!(repeats <= 10)) {
          return cli_replyFailure(td);
        }
      }
        break;

      case otok::p: {
        NODEFAULT();
        if (is_val("?")) {
          has_requested_position = true;
        } else {
          pct = atoi(val);
          if (!(0 <= pct && pct <= 100))
            return cli_replyFailure(td);
        }
      }
        break;

      case otok::c: {
        NODEFAULT();
        if (is_val("?")) {
          has_requested_position = true;
        } else if (!cli_parm_to_ferCMD(val, &cmd)) {
          return cli_replyFailure(td);
        }
      }
        break;

      case otok::SEP: {
        set_end_pos = asc2bool(val);
        if (set_end_pos != 1)
          set_end_pos = 0;  // force disable
      }
        break;

      default:
        cli_warning_optionUnknown(td, key);
        break;
      }
    } else {
      cli_warning_optionUnknown(td, key);
    }
  }

  if (has_requested_position) {
    if (g != 0) {
      int pos = fer_simPos_getPct_whileMoving(addr, g, m);
      if (pos >= 0) {
        so_arg_gmp_t gmp = {g, m, pos};
        soMsg_pos_print_gmp(td, gmp);
      }
    } else {
      fer_statPos_printAllPcts(td);
    }
  } else {
    if (has_sep) { // enable hardware buttons to set end position
      if (set_end_pos)
        fer_sep_enable(td, fer_get_fsb(addr,g,m,cmd));
      else
        fer_sep_disable();
    } else if (has_pct) {
      cli_replyResult(td, fer_cmd_moveShutterToPct(addr, g, m, pct, repeats));
    } else if (has_cmd) {
      cli_replyResult(td, fer_cmd_sendShutterCommand(addr, g, m, cmd, repeats));
    } else {
      cli_replyFailure(td);
    }
  }
  return 0;
}


