#include "app/proj_app_cfg.h"

#include <string.h>

#include "fernotron_sep/set_endpos.h"
#include "fernotron_pos/commands.h"
#include "fernotron_pos/shutter_pct.h"
#include "txtio/inout.h"
#include "app/proj_app_cfg.h"
#include "fernotron_auto/timer_state.h"
#include "fernotron/fer_rx_tx.h"
#include "fernotron/fer_msg_extension.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "userio_app/status_output.h"
#include "config/config.h"
#include "cli_fer.h"


#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

const char help_parmSend[]  =
    "a=(0|ID)  0  hex ID of sender or receiver.\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  group member number\n"
    "c=(up|down|stop|sun-down|sun-inst|set|?)\n"
    "p=?          get current position\n"
    "r=N       2  repeat command 1+N times\n"
    "SEP[=0|1]    Enter end-position adjustment mode (needs hardware button)"
// "TROT        Toggle rotation direction"
;

int 
process_parmSend(clpar p[], int len) {
  int arg_idx;

  u32 addr = C.fer_centralUnitID;
  u8 g = 0, m = 0;
  fer_cmd cmd = fer_cmd_None;
  int set_end_pos = -1;
  u8 repeats = FSB_PLAIN_REPEATS;
  bool has_requested_position = false;
  i8 pct = -1;
#define has_pct (pct >= 0)
#define has_cmd (cmd != fer_cmd_None)
#define has_sep (set_end_pos >= 0)


  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, "a") == 0) {
      u32 tmp = val ? strtol(val, NULL, 16) : 0;
      if (tmp) addr = tmp;
    } else if (strcmp(key, "g") == 0) {
      int arg = atoi(val);
      if (0 <= arg && arg <= 7) {
         g = arg;
      } else {
        return reply_failure();
      }
    } else if (strcmp(key, "m") == 0) {
      int arg = atoi(val);
      if (0 <= arg && arg <= 7) {
         m = arg;
      } else {
        return reply_failure();
      }
    } else if (strcmp(key, "r") == 0) {
      NODEFAULT();
      repeats = atoi(val);
      if (!(repeats <= 10)) {
        return reply_failure();
      }
    } else if (strcmp(key, "p") == 0) {
      NODEFAULT();
      if (*val == '?') {
        has_requested_position = true;
      } else {
        pct = atoi(val);
        if (!(0 <= pct && pct <= 100))
          return reply_failure();
      }
    } else if (strcmp(key, "c") == 0) {
      NODEFAULT();
      if (*val == '?') {
        has_requested_position = true;
      } else if (!cli_parm_to_ferCMD(val, &cmd)) {
        return reply_failure();
      }
    } else if (strcmp(key, "SEP") == 0) {
      set_end_pos = asc2bool(val);
      if (set_end_pos != 1)
      set_end_pos = 0;  // force disable
    } else {
      warning_unknown_option(key);
    }
  }



  if (has_requested_position) {
    if (g != 0 && m != 0) {
      int pos = ferPos_getPct_whileMoving(addr, g, m);
      if (pos >= 0) {
        so_arg_gmp_t gmp = {g, m, pos};
        so_output_message(SO_POS_PRINT_GMP, &gmp);
      }
    } else {
      ferPos_printPctsAll();
    }
  } else {
    if (has_sep) { // enable hardware buttons to set end position
      if (set_end_pos)
        sep_enable(get_fsb(addr,g,m,cmd));
      else
        sep_disable();
    } else if (has_pct) {
      commands_moveShutterToPct(addr, g, m, pct, repeats);
    } else if (has_cmd) {
      reply(commands_sendShutterCommand(addr, g, m, cmd, repeats));
    } else {
      reply_failure();
    }
  }
  return 0;
}


