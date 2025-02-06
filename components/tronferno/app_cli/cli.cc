/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */


#include <string.h>
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "utils_misc/bcd.h"
#include "cli_internal.hh"
#include <app_cli/cli_app.hh>
#include "uout/uout_builder_json.hh"
#include "cli/cli.h"
#include "app_uout/status_output.h"
#include "app_settings/config.h"
#include <fernotron_trx/fer_trx_c_api.h>
#include <fernotron/sep/set_endpos.h>
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
    { "sep-down", fer_if_cmd_EndPosDOWN }, //
    { "sep-up", fer_if_cmd_EndPosUP }, //
    { "stop", fer_if_cmd_STOP }, //
    { "sun-down", fer_if_cmd_SunDOWN }, //
    { "sun-up", fer_if_cmd_SunUP }, //
    { "sun-inst", fer_if_cmd_SunINST }, //
    { "sun-pos", fer_if_cmd_SunINST }, //
    {"sun-test", fer_if_cmd_Program},//
    { "set", fer_if_cmd_SET },  //
    { "rot-dir", fer_if_cmd_ToggleRotationDirection },  //
    };

bool cli_parm_to_ferCMD(const char *token, fer_if_cmd *cmd) {
  int i;
  fer_if_cmd result;

  for (i = 0; i < (sizeof(fc_map) / sizeof(fc_map[0])); ++i) {
    if (strcmp(token, fc_map[i].fs) == 0) {
      result = fc_map[i].fc;
      *cmd = result;
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
  config_save_item_n_i8(comp_sett.get_kvsKey(CB_RECV), C.app_recv);
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
  config_save_item_n_i8(comp_sett.get_kvsKey(CB_TRANSM), C.app_transm);
  return true;
}

bool asc2u8(const char *s, uint8_t *n, uint8_t limit) {
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
"type 'help <command>;'  or 'help all;'\nThe commands are: ";

static struct parm_handler const handlers[] = { //
#ifdef CONFIG_APP_USE_SEP
        { "sep", process_parmSep, cli_help_parmSep }, //
#endif
        { "cmd", process_parmSend, cli_help_parmSend },
        { "config", process_parmConfig, cli_help_parmConfig }, //
        { "mcu", process_parmMcu, cli_help_parmMcu }, //
        { "auto", process_parmTimer, cli_help_parmTimer },
        { "help", process_parmHelp, cli_help_parmHelp }, //
#ifdef CONFIG_APP_USE_PAIRINGS
        { "pair", process_parmPair, cli_help_parmPair},//
#endif
        { "shpref", process_parmShpref, cli_help_parmShpref }, //
        { "kvs", process_parmKvs, cli_help_parmKvs }, //
    };

static const struct parm_handlers our_parm_handlers = { .handlers = handlers, .count = sizeof(handlers) / sizeof(handlers[0]), };

const parm_handler* cli_parmHandler_find(const char *key) {
#ifdef CONFIG_APP_USE_SEP
  // While in SEP mode reserve the MCU and RF for SEP commands only
  if (fer_sep_is_enabled()) {
    if (strcmp("sep", key) == 0)
      return &handlers[0];
    return nullptr;
  }
#endif
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

static bool cliApp_checkPassword(clpar p[], int len, class UoutWriter &td) {
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



