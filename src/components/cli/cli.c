/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "cli_app_cfg.h"
#include "misc/bcd.h"
#include "cli.h"
#include "userio/status_output.h"
#include "txtio/inout.h"
#include "mutex.h"
#include "userio/status_json.h"
#include "debug/debug.h"

#include <string.h>

u16 cli_msgid;
bool cli_isJson;

int asc2bool(const char *s) {
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

void cli_replySuccess() {
  reply_message(0, "ok");
}

int cli_replyFailure() {
  reply_message(0, "error");
  return -1;
}

bool cli_replyResult(bool success) {
  if (success)
    cli_replySuccess();
  else
    cli_replyFailure();
  return success;
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

void cli_loop(void) {
  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    if (mutex_cliTake()) {
      if (cmdline[0] == '{') {
        cli_process_json(cmdline, SO_TGT_CLI);
      } else {

        io_putlf();
        cli_process_cmdline(cmdline, SO_TGT_CLI);
        cli_msg_ready();
      }

      mutex_cliGive();
    }
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}

