/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app/proj_app_cfg.h"


#include <string.h>

#include "fernotron_sep/set_endpos.h"
#include "fernotron_pos/shutter_pct.h"
#include "config/config.h"
#include "fernotron_auto/timer_state.h"
#include "misc/bcd.h"
#include "cli_imp.h"
//#include "userio_app/status_output.h"
#include "fernotron_auto/timer_data.h"
#include "cli_app.h"
#include "userio/status_json.h"

#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)
#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

fsbT default_sender;

u16 msgid;


struct c_map {
  const char *fs;
  fer_cmd fc;
};

struct c_map const fc_map[] = { { "down", fer_cmd_DOWN }, { "up", fer_cmd_UP },
                                { "stop", fer_cmd_STOP }, { "sun-down", fer_cmd_SunDOWN },
                                { "sun-up", fer_cmd_SunUP }, { "sun-inst", fer_cmd_SunINST },
                                //		{"sun-test", fer_cmd_Program},
                                { "set", fer_cmd_SET }, };

bool 
cli_parm_to_ferCMD(const char *token, fer_cmd *cmd) {
  int i;
  dbg_trace();

  for (i = 0; i < (sizeof(fc_map) / sizeof(fc_map[0])); ++i) {
    if (strcmp(token, fc_map[i].fs) == 0) {
      *cmd = fc_map[i].fc;
      return true;
    }
  }
  return false;
}

bool  config_receiver(const char *val) {
  if (strcmp(val, "on") == 0) {
    C.app_recv = recvTick;
  } else if (strcmp(val, "off") == 0) {
    C.app_recv = recvNone;
  } else {
    return false;
  }
  save_config_item(CB_RECV);
  return true;
}

bool 
config_transmitter(const char *val) {
  if (strcmp(val, "on") == 0) {
    C.app_transm = transmTick;
  } else if (strcmp(val, "off") == 0) {
    C.app_transm = transmNone;
  } else {
    return false;
  }
  save_config_item(CB_TRANSM);
  return true;
}

fsbT * 
get_sender_by_addr(long addr) {
  if (addr == 0) {
    return &default_sender;
  } else if (addr > 0 && 10 > addr) {
    //return &senders[addr];
  }
  return NULL ;
}




bool  asc2group(const char *s, fer_grp *grp) {
  if (s) {
    int g = atoi(s);
    if (0 <= g && g <= 7) {
      *grp = g;
      return true;
    }
  }
  return false;
}

bool  asc2memb(const char *s, fer_memb *memb) {
  if (s) {

    int m = atoi(s);
    if (0 <= m && m <= 7) {
      *memb = (m == 0 ? fer_memb_Broadcast : (fer_memb) (m - 1) + fer_memb_M1);
      return true;
    }
  }
  return false;
}

int process_parmHelp(clpar p[], int len);

const char help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";


static struct parm_handler handlers[] = {
                      { "send", process_parmSend, help_parmSend },
                      { "cmd", process_parmSend, help_parmSend }, // alias
                      { "config", process_parmConfig, help_parmConfig },
                      { "mcu", process_parmMcu, help_parmMcu },
                      { "timer", process_parmTimer, help_parmTimer },
                      { "auto", process_parmTimer, help_parmTimer }, // alias
                      { "help", process_parmHelp, help_parmHelp },
#ifdef USE_PAIRINGS
                      { "pair", process_parmPair, help_parmPair},
#endif
                      { "shpref", process_parmShpref, help_parmShpref},
  };

const struct parm_handlers parm_handlers = {
    .handlers = handlers,
    .count = sizeof(handlers) / sizeof(handlers[0]),
};


bool cli_isJson;

#if TEST_MODULE_CLI
bool testModule_cli() {
  char cl[] = "timer g=5 astro=0;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  if (n > 0)
    if (sj_open_root_object("tfmcu")) {
      process_parm(par, n);
      sj_close_root_object();
    }
  return n != -1;
}
#endif
