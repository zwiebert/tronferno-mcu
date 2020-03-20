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


int ENR; // error number
void  print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

void  msg_print(const char *msg, const char *tag) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  if (msg)
    io_puts(msg);
  if (cli_msgid) {
    io_putc('@');
    io_putd(cli_msgid);
  }
  if (tag) {
    io_putc(':');
    io_puts(tag);
  }
  io_puts(": ");
}

void  cli_warning_optionUnknown(const char *key) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

void  cli_reply_print(const char *tag) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  msg_print("cli_replyResult", tag);
}

void  reply_message(const char *tag, const char *msg) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  cli_reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void  cli_msg_ready(void) {
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;
  io_puts("\nready:\n");
}

void  reply_id_message(u16 id, const char *tag, const char *msg) {
  u16 old_id = cli_msgid;
  if (!so_tgt_test(SO_TGT_CLI) || cli_isJson)
    return;

  cli_msgid = id;
  cli_reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
  cli_msgid = old_id;
}
