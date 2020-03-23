/*
 * cmdline_process_cli.c
 *
 *  Created on: 18.03.2020
 *      Author: bertw
 */

#include "cli_app_cfg.h"
#include "misc/bcd.h"
#include "cli.h"
#include "cli_json.h"
#include "userio/status_output.h"
#include "txtio/inout.h"
#include "mutex.h"
#include "userio/status_json.h"
#include "debug/debug.h"

#include <string.h>

int process_parmHelp(clpar p[], int len);



int cli_processParameters(clpar p[], int len) {
  int i;
  int result = -1;

  for (i = 0; i < parm_handlers.count; ++i) {
    if (strcmp(p[0].key, parm_handlers.handlers[i].parm) == 0) {
      result = parm_handlers.handlers[i].process_parmX(p, len);
      break;
    }
  }
  return result;
}


void cli_process_json(char *json, so_target_bits tgt) {
  cli_isJson = true;

  so_tgt_set(tgt);

  dbg_vpf(db_printf("process_json: %s\n", json));

  char *name, *cmd_obj;

  if (sj_open_root_object("tfmcu")) {
    while ((cmd_obj = json_get_command_object(json, &name, &json))) {
      int n = parse_json(name, cmd_obj);
      if (n < 0) {
        cli_replyFailure();
      } else {
        cli_processParameters(cli_par, n);
      }
    }
    sj_close_root_object();
  }

  if (so_tgt_test(SO_TGT_CLI)) {
    cli_print_json(sj_get_json());
  }

  so_tgt_default();
}



void cli_process_cmdline(char *line, so_target_bits tgt) {
  dbg_vpf(db_printf("process_cmdline: %s\n", line));
  cli_isJson = false;
  so_tgt_set(tgt);

  int n = cli_parseCommandline(line);
  if (n < 0) {
    cli_replyFailure();
  } else {
    if (sj_open_root_object("tfmcu")) {
      cli_processParameters(cli_par, n);
      sj_close_root_object();
    }
  }
  so_tgt_default();
}
