/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "cli_app_cfg.h"


#include <string.h>

#include "misc/bcd.h"
#include "cli_priv.h"
#include "cli.h"
#include "userio/status_output.h"
#include "txtio/inout.h"
#include "mutex.h"
#include "fernotron_sep/set_endpos.h" // XXX

#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)
#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

u16 msgid;

typedef void (*void_fun_ptr)(void);

int
asc2bool(const char *s) {
  if (!s)
    return 1; // default value for key without value

  switch (*s) {
  case '0':
    return 0;
  case '1':
    return 1;
  default:
    return -1;
  }

}

void
reply_success() {
  reply_message(0, "ok");
}

int
reply_failure() {
  reply_message(0, "error");
  return -1;
}



bool
reply(bool success) {
  if (success)
    reply_success();
  else
    reply_failure();
  return success;
}


bool  asc2u8(const char *s, u8 *n, u8 limit) {
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


extern const char help_parmHelp[];

int
process_parm(clpar p[], int len) {
  int i;
  int result = -1;

  if (mutex_cliTake()) {

    // if in sep mode, don't accept commands FIXME
    if (sep_is_enabled()) {
      sep_disable();
      reply_message(0, "error: CLI is disabled in set-endposition-mode\n");
    } else {
      for (i = 0; i < parm_handlers.count; ++i)  {
        if (strcmp(p[0].key, parm_handlers.handlers[i].parm) == 0) {
          result = parm_handlers.handlers[i].process_parmX(p, len);
          break;
        }
      }
    }
    mutex_cliGive();
  }

  return result;
}

void
cli_process_cmdline(char *line) {
  dbg_vpf(db_printf("process_cmdline: %s\n", line));
  cli_isJson = false;

  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}

int
process_parmHelp(clpar p[], int len) {
  int i;

  static const char usage[]  = "syntax: command option=value ...;\n"
      "commands are: ";

  io_putlf();

  // print help for help;
  if (len == 1) {
    io_puts(help_parmHelp);
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
        warning_unknown_option(key);
      }
    }
  }

  io_puts("\ncommon options:\n"
      "mid=N  N is used as an ID in the reply\n");

  return 0;
}

bool cli_isJson;


void  cli_loop(void) {
  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    so_tgt_set(SO_TGT_CLI);
    if (cmdline[0] == '{') {

     cli_process_json(cmdline);
    } else {

      io_putlf();
      cli_process_cmdline(cmdline);
      cli_msg_ready();
    }
    so_tgt_default();
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}

