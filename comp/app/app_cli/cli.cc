/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app/config/proj_app_cfg.h"
#include <string.h>
#include "app/settings/config.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "app/cli/cli_app.h"
#include "uout/status_json.hh"
#include "cli/cli.h"
#include "app/uout/status_output.h"
#include "app/settings/config.h"
#include <fernotron/api/fer_msg_send.hh>
#include <algorithm>
#include <iterator>

#define CI(cb) static_cast<configItem>(cb)



struct c_map {
  const char *fs;
  fer_if_cmd fc;
};

struct c_map const fc_map[] = { //
    { "down", fer_if_cmd_DOWN }, //
    { "up", fer_if_cmd_UP }, //
    { "stop", fer_if_cmd_STOP }, //
    { "sun-down", fer_if_cmd_SunDOWN }, //
    { "sun-up", fer_if_cmd_SunUP }, //
    { "sun-inst", fer_if_cmd_SunINST }, //
    //{"sun-test", fer_if_cmd_Program},//
    { "set", fer_if_cmd_SET },  //
    };

bool 
cli_parm_to_ferCMD(const char *token, fer_if_cmd *cmd) {
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
  config_save_item_n_i8(CI(CB_RECV), C.app_recv);
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
  config_save_item_n_i8(CI(CB_TRANSM), C.app_transm);
  return true;
}

bool asc2u8(const char *s, u8 *n, u8 limit) {
  if (s) {
    int g = atoi(s);
    if (0 <= g && g <= limit) {
      *n = g;
      return true;
    }
  }
  return false;
}

int process_parmHelp(clpar p[], int len);

const char cli_help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";

static struct parm_handler const handlers[] = { //
        { "cmd", process_parmSend, cli_help_parmSend },
        { "config", process_parmConfig, cli_help_parmConfig }, //
        { "mcu", process_parmMcu, cli_help_parmMcu }, //
        { "auto", process_parmTimer, cli_help_parmTimer },
        { "help", process_parmHelp, cli_help_parmHelp }, //
#ifdef USE_PAIRINGS
        { "pair", process_parmPair, cli_help_parmPair},//
#endif
        { "shpref", process_parmShpref, cli_help_parmShpref }, //
    };

static const struct parm_handlers our_parm_handlers = { .handlers = handlers, .count = sizeof(handlers) / sizeof(handlers[0]), };

const parm_handler* cli_parmHandler_find(const char *key) {
  auto handler = std::find_if(std::begin(handlers), std::end(handlers), [&key](auto el) {
    return strcmp(key, el.parm) == 0;
  });
  if (std::end(handlers) == handler) {
    if (strcmp("timer", key) == 0)  // alias
      return cli_parmHandler_find("auto");
    if (strcmp("send", key) == 0)  // alias
      return cli_parmHandler_find("cmd");

    return nullptr;
  }

  return handler;
}

/////////////// setup //////////////////

static bool cliApp_checkPassword(clpar p[], int len, const struct TargetDesc &td) {
  if (len < 2)
    return true;

  if (strcmp(p[0].key, "config") != 0)
    return true;

  if (!C.app_configPassword[0])
    return true;
  if (strcmp(p[1].key, "pw") == 0) {
    if (strcmp(p[1].val, C.app_configPassword) == 0) {
      soMsg_cfgpasswd_ok(td);
      return true;
    } else {
      soMsg_cfgpasswd_wrong(td);
    }
  } else {
    soMsg_cfgpasswd_missing(td);
  }

  return false;
}


void cliApp_setup() {
  cli_hook_checkPassword = cliApp_checkPassword;
  cli_parmHandler_find_cb = cli_parmHandler_find;
  cli_parm_handlers = &our_parm_handlers;
}



