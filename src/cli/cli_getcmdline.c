#include "user_config.h"
#include "userio/inout.h"
#include "cli_imp.h"

char cmd_buf[CMD_BUF_SIZE];

char * ICACHE_FLASH_ATTR
get_commandline() {
  char *result = NULL;

  static int cmd_buf_idx;
  static bool error;
  int c;
  static int quoteCount;

  while ((c = io_getc()) != -1) {

    // line ended before ';' terminator received. throw it away
    if (c == '\r' || c == '\n') {
      quoteCount = 0;
      cmd_buf_idx = 0;
      continue;
    }

    // backspace: remove last character from buffer
    if (c == '\b') {
      if (cmd_buf_idx == 0)
        continue;
      if (cmd_buf[--cmd_buf_idx] == '\"')
        --quoteCount;
      continue;
    }

    // count the quotes, so we know if we are inside or outside a quoted word
    if (c == '\"') {
      ++quoteCount;
    }

    // to throw away rest of a too long command line from earlier
    if (error) {
      if (c == ';')
        error = false;
      continue;
    }
    // to make sure there is at least 1 free byte available in CMD_BUF
    if (!((cmd_buf_idx + 1) < CMD_BUF_SIZE)) {
      goto err;
    }

    // handle special characters, if outside a quoted word
    if ((quoteCount & 1) == 0) {
      // ';' is used to terminate a command line
      if (c == ';' && (quoteCount & 1) == 0) {
        if (cmd_buf_idx == 0)
          goto succ;
        cmd_buf[cmd_buf_idx] = '\0';
        result = cmd_buf;
        goto succ;
      }
    }

    // store char to buffer
    cmd_buf[cmd_buf_idx++] = (char) c;
  }

  goto cont;

  err: error = true;
  succ: cmd_buf_idx = 0;
  quoteCount = 0;
  cont: return result;
}
