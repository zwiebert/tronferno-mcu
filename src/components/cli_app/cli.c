/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "config/config.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "cli_app.h"
#include "userio/status_json.h"
#include "fernotron/fsb.h"


fsbT default_sender;

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

const char cli_help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";


static struct parm_handler handlers[] = {
                      { "send", process_parmSend, cli_help_parmSend },
                      { "cmd", process_parmSend, cli_help_parmSend }, // alias
                      { "config", process_parmConfig, cli_help_parmConfig },
                      { "mcu", process_parmMcu, cli_help_parmMcu },
                      { "timer", process_parmTimer, cli_help_parmTimer },
                      { "auto", process_parmTimer, cli_help_parmTimer }, // alias
                      { "help", process_parmHelp, cli_help_parmHelp },
#ifdef USE_PAIRINGS
                      { "pair", process_parmPair, cli_help_parmPair},
#endif
                      { "shpref", process_parmShpref, cli_help_parmShpref},
  };

const struct parm_handlers parm_handlers = {
    .handlers = handlers,
    .count = sizeof(handlers) / sizeof(handlers[0]),
};


bool cli_isJson;


