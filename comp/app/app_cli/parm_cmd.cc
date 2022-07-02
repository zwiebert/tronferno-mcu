#include "app_config/proj_app_cfg.h"

#include <string.h>
#include "fernotron/fer_main.h"
#include "fernotron/pos/commands.h"
#include "fernotron/pos/shutter_pct.h"
#include "txtio/inout.h"
#include "app_config/proj_app_cfg.h"
#include "fernotron/auto/fau_tminutes.h"
#include "utils_misc/bcd.h"
#include "cli_imp.h"
#include "app_uout/status_output.h"
#include "app_settings/config.h"
#include "cli_fer.h"
#include "app_misc/opt_map.hh"
#include <stdlib.h>
#include <fernotron_trx/fer_trx_c_api.h>

#define FER_SB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

const char cli_help_parmSend[]  = "'cmd' sends a command to a receiver (or get current position)\n\n"
    "a=(0|ID)  0  transmitter ID (6 digit hex). 0 for programming center.\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  receiver number\n"
    "c=(up|down|stop|sun-down|sun-inst|sun-pos|sun-test|set|rot-dir|?) Command. '?' gets current pos\n"
    "p=?          get current position in percent\n"
    "r=N       2  repeat command 1+N times\n"
;

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

int
process_parmSend(clpar p[], int len, const struct TargetDesc &td) {
  int arg_idx;

  u32 addr = fer_config.cu;
  u8 g = 0, m = 0;
  fer_if_cmd cmd = fer_if_cmd_None;
  u8 repeats = FER_SB_PLAIN_REPEATS;
  bool has_requested_position = false;
  i8 pct = -1;
#define has_pct (pct >= 0)
#define has_cmd (cmd != fer_if_cmd_None)

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (!key)
      return -1;

    otok kt = optMap_findToken(key);

    if (kt != otok::NONE) {
      switch (kt) {
      case otok::k_a: {
        u32 tmp = val ? strtol(val, NULL, 16) : 0;
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

      case otok::k_r: {
        NODEFAULT();
        repeats = atoi(val);
        if (!(repeats <= 10)) {
          return cli_replyFailure(td);
        }
      }
        break;

      case otok::k_p: {
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

      case otok::k_c: {
        NODEFAULT();
        if (is_val("?")) {
          has_requested_position = true;
        } else if (!cli_parm_to_ferCMD(val, &cmd)) {
          return cli_replyFailure(td);
        }
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
    if (addr != fer_config.cu) {
      return cli_replyFailure(td);
    } else if (g != 0) {
      if (Pct pos = fer_simPos_getPct_whileMoving(g, m)) {
        so_arg_gmp_t gmp = {g, m, pos};
        soMsg_pos_print_gmp(td, gmp);
      }
    } else {
      fer_statPos_printAllPcts(td);
    }

  } else {
   if (has_pct) {
      cli_replyResult(td, fer_cmd_moveShutterToPct(addr, g, m, pct, repeats));
    } else if (has_cmd) {
      cli_replyResult(td, fer_cmd_sendShutterCommand(addr, g, m, cmd, cmd == fer_if_cmd_ToggleRotationDirection ? 0 : repeats));
    } else {
      cli_replyFailure(td);
    }
  }
  return 0;
}


