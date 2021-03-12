/*
 * json_parse.c
 *
 *  Created on: 18.03.2020
 *      Author: bertw
 */

#include "cli/cli.h"
extern "C++" {
#include <uout/cli_out.h>
}
#include "txtio/inout.h"
#include "debug/dbg.h"
#include "cli_imp.h"
#include <string.h>

int process_parmHelp(clpar p[], int len, const struct TargetDesc &td) {
  int i;


  static const char usage[] = "syntax: command option=value ...;\n"
      "commands are: ";

  td << '\n';

  if (!cli_parm_handlers) {
    td << "no commands available\n";
    return -1;
  }

  // print help for help;
  if (len == 1) {
    td << cli_help_parmHelp;
    for (i = 0; i < cli_parm_handlers->count; ++i) {
      td << cli_parm_handlers->handlers[i].parm << ", ";
    }
    td << '\n';
    return 0;
  }

  // print help for help command; or help all;
  for (i = 1; i < len; ++i) {
    int k;
    const char *key = p[i].key; //, *val = p[i].val;
    bool keyProcessed = false;

    if (strcmp(key, "all") == 0) {
      td << usage;
      for (i = 0; i < cli_parm_handlers->count; ++i) {
        td << cli_parm_handlers->handlers[i].parm << ", ";
      }
      td << '\n';

      for (i = 0; i < cli_parm_handlers->count; ++i) {
        td << cli_parm_handlers->handlers[i].parm << " options:\n";
        td << cli_parm_handlers->handlers[i].help << '\n';
      }

    } else {

      for (k = 0; k < cli_parm_handlers->count; ++k) {
        if (strcmp(cli_parm_handlers->handlers[k].parm, key) == 0) {
          td << cli_parm_handlers->handlers[k].help << '\n';
          keyProcessed = true;
          break;
        }
      }

      if (!keyProcessed) {
        cli_warning_optionUnknown(td, key);
      }
    }
  }

  td << "\ncommon options:\n"
      "mid=N  N is used as an ID in the cli_reply\n";

  return 0;
}
