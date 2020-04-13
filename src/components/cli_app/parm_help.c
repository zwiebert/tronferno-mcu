/*
 * json_parse.c
 *
 *  Created on: 18.03.2020
 *      Author: bertw
 */

#include "cli/cli.h"
#include "txtio/inout.h"
#include "debug/debug.h"
#include "cli_app/cli_imp.h"

int process_parmHelp(clpar p[], int len) {
  int i;

  static const char usage[] = "syntax: command option=value ...;\n"
      "commands are: ";

  io_putlf();

  // print help for help;
  if (len == 1) {
    io_puts(cli_help_parmHelp);
    for (i = 0; i < parm_handlers.count; ++i) {
      io_puts(parm_handlers.handlers[i].parm), io_puts(", ");
    }
    io_putlf();
    return 0;
  }

  // print help for help command; or help all;
  for (i = 1; i < len; ++i) {
    int k;
    const char *key = p[i].key; //, *val = p[i].val;
    bool keyProcessed = false;

    if (strcmp(key, "all") == 0) {
      io_puts(usage);
      for (i = 0; i < parm_handlers.count; ++i) {
        io_puts(parm_handlers.handlers[i].parm), io_puts(", ");
      }
      io_putlf();

      for (i = 0; i < parm_handlers.count; ++i) {
        io_puts(parm_handlers.handlers[i].parm), io_puts(" options:\n");
        io_puts(parm_handlers.handlers[i].help), io_putlf();
      }

    } else {

      for (k = 0; k < parm_handlers.count; ++k) {
        if (strcmp(parm_handlers.handlers[k].parm, key) == 0) {
          io_puts(parm_handlers.handlers[k].help), io_putlf();
          keyProcessed = true;
          break;
        }
      }

      if (!keyProcessed) {
        cli_warning_optionUnknown(key);
      }
    }
  }

  io_puts("\ncommon options:\n"
      "mid=N  N is used as an ID in the cli_replyResult\n");

  return 0;
}
