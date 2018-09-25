/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../user_config.h"
#include "current_state.h"
#include "timer_state.h"

#include "all.h"
#include "set_endpos.h"
#include "misc/bcd.h"
#include "cli_imp.h"

#if defined  MCU_ESP8266 || defined MCU_ESP32
#include "timer_data.h"
#define ENABLE_RSTD 1
#else
#define ENABLE_RSTD 0
#endif

#define ENABLE_EXPERT 0 // work in progress
#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

//fer_sender_basic senders[10];
fer_sender_basic default_sender;
fer_sender_basic last_received_sender;
uint16_t msgid;

int ENR; // error number
/*static*/ void ICACHE_FLASH_ATTR print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

/*static*/ void ICACHE_FLASH_ATTR msg_print(const char *msg, const char *tag) {
  if (msg)
    io_puts(msg);
  if (msgid) {
    io_putc('@');
    io_putd(msgid);
  }
  if (tag) {
    io_putc(':');
    io_puts(tag);
  }
  io_puts(": ");
}

void ICACHE_FLASH_ATTR warning_unknown_option(const char *key) {
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

/*static*/ void ICACHE_FLASH_ATTR reply_print(const char *tag) {
  msg_print("reply", tag);
}

/*static*/ void ICACHE_FLASH_ATTR reply_message(const char *tag, const char *msg) {
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void ICACHE_FLASH_ATTR cli_msg_ready(void) {
  io_puts("\nready:\n");
}


char * ICACHE_FLASH_ATTR
get_commandline() {
  char *result = NULL;
#define CMD_BUF_SIZE 128
  static char cmd_buf[CMD_BUF_SIZE];
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


#define MAX_PAR 10
clpar par[MAX_PAR];

/*static*/ char * ICACHE_FLASH_ATTR
skip_leading_whitespace(char *s) {
  while (*s == ' ')
    *s++ = '\0';

  return s;
}

/*static*/ char * ICACHE_FLASH_ATTR
find_next_space_eq_eol(char *s) {
  while (*s != '\0' && *s != ' ' && *s != '=') {
    s++;
  }
  return s;
}
/*static*/ char * ICACHE_FLASH_ATTR
find_next_whitespace_or_eol(char *s) {
  while (*s != '\0' && *s != ' ' && *s != '=') {
    s++;
  }
  return s;
}

int ICACHE_FLASH_ATTR
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

int ICACHE_FLASH_ATTR
parse_commandline(char *cl) {
  int p;
  msgid = 0;

  for (p = 0; p < MAX_PAR; ++p) {
    bool isValQuoted = false;

    // trim
    cl = skip_leading_whitespace(cl);

    // eol
    if (*cl == '\0') {
      return p;
    }

    // first word is the key
    par[p].key = cl;

    // skip the key
    cl = find_next_space_eq_eol(cl);

    switch (*cl) {

    // no val follows the key. set val to NULL (interpreted as default)
    case '\0':
    case ' ':
      par[p].val = NULL;
      continue;

      // got val for the key
    case '=':
      // add null terminator to key
      *cl++ = '\0';
      if (*cl == ';' || *cl == ' ') {
        return -1;
      }

      if (*cl == '\"') {
        isValQuoted = true;
        ++cl;
      }

      // eol - error
      if (*cl == '\0') {
        return -1;
      }

      // start of val
      par[p].val = cl;

      // end of val
      if (isValQuoted) {
        char *q = strchr(cl, '\"');
        // quote unbalanced
        if (q == 0) {
          return -1;
        }
        cl = q;

      } else {
        cl = find_next_whitespace_or_eol(cl);
      }

      // terminate val if not already terminated by eol
      if (*cl != '\0') {
        *cl++ = '\0';
      }

      // found global option
      // process it here and remove the key/val
      if (strcmp(par[p].key, "mid") == 0) {
        msgid = atoi(par[p].val);
        --p;
      }

      break;
    }
  }

  return -1;

}

struct c_map {
  const char *fs;
  fer_cmd fc;
};

struct c_map const fc_map[] = { { "down", fer_cmd_DOWN }, { "up", fer_cmd_UP }, { "stop", fer_cmd_STOP }, { "sun-down", fer_cmd_SunDOWN }, { "sun-up",
    fer_cmd_SunUP }, { "sun-inst", fer_cmd_SunINST },
//		{"sun-test", fer_cmd_Program},
    { "set", fer_cmd_SET }, };

bool ICACHE_FLASH_ATTR
cli_parm_to_ferCMD(const char *token, fer_cmd *cmd) {
  int i;
  dbg_trace();

  for (i = 0; i < (sizeof(fc_map) / sizeof(fc_map[0])); ++i) {
    if (strcmp(token, fc_map[i].fs) == 0) {
      *cmd = fc_map[i].fc;
      return true;
    }
  }
  return false;
}

void ICACHE_FLASH_ATTR
reply_success() {
  reply_message(0, "ok");
}

int ICACHE_FLASH_ATTR
reply_failure() {
  reply_message(0, "error");
  return -1;
}

bool ICACHE_FLASH_ATTR config_receiver(const char *val) {
  if (strcmp(val, "on") == 0) {
    C.app_recv = recvTick;
  } else if (strcmp(val, "off") == 0) {
    C.app_recv = recvNone;
  } else {
    return false;
  }
  save_config(CONFIG_RECV);
  return true;
}

bool ICACHE_FLASH_ATTR
config_transmitter(const char *val) {
  if (strcmp(val, "on") == 0) {
    C.app_transm = transmTick;
  } else if (strcmp(val, "off") == 0) {
    C.app_transm = transmNone;
  } else {
    return false;
  }
  save_config(CONFIG_TRANSM);
  return true;
}

bool ICACHE_FLASH_ATTR
reply(bool success) {
  if (success)
    reply_success();
  else
    reply_failure();
  return success;
}

fer_sender_basic * ICACHE_FLASH_ATTR
get_sender_by_addr(long addr) {
  if (addr == 0) {
    return &default_sender;
  } else if (addr > 0 && 10 > addr) {
    //return &senders[addr];
  }
  return NULL ;
}


bool ICACHE_FLASH_ATTR asc2group(const char *s, fer_grp *grp) {
  if (s) {
    int g = atoi(s);
    if (0 <= g && g <= 7) {
      *grp = g;
      return true;
    }
  }
  return false;

  return (fer_grp) atoi(s);
}

bool ICACHE_FLASH_ATTR asc2memb(const char *s, fer_memb *memb) {
  if (s) {

    int m = atoi(s);
    if (0 <= m && m <= 7) {
      *memb = (m == 0 ? fer_memb_Broadcast : (fer_memb) (m - 1) + fer_memb_M1);
      return true;
    }
  }
  return false;
}



bool ICACHE_FLASH_ATTR
string2bcdArray(const char *src, uint8_t *dst, uint16_t size_dst) {
  char buf[3];
  int i;

  buf[2] = 0;

  for (i = 0; i < size_dst && src[0]; ++i) {
    switch (*src) {
    case '-': // timer off
      dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
      if (!(++i < size_dst))
        return false;
      ;
      dst[i] = (i % 2 == 0) ? 0xff : 0x0f;
      ++src;
      break;
    case '+': // copy previous day on+off timer
      if (i < FPR_TIMER_STAMP_WIDTH || i + FPR_TIMER_STAMP_WIDTH > size_dst)
        return false;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH], ++i;
      dst[i] = dst[i - FPR_TIMER_STAMP_WIDTH];
      ++src;
      break;

    default:
      buf[0] = *src++;
      buf[1] = *src++;
      if (!(++i < size_dst))
        return false;
      ;
      dst[i] = dec2bcd(atoi(buf));
      buf[0] = *src++;
      buf[1] = *src++;
      dst[i - 1] = dec2bcd(atoi(buf));

    }

  }

  return i == size_dst;
}

#if ENABLE_EXPERT

struct c_map const ec_map[] = {
  { "down", fer_cmd_DOWN},
  { "up", fer_cmd_UP},
  { "stop", fer_cmd_STOP},
  { "sun-down", fer_cmd_SunDOWN},
  { "sun-up", fer_cmd_SunUP},
  { "sun-inst", fer_cmd_SunINST},
//		{"sun-test", fer_cmd_Program},
  { "set", fer_cmd_SET},
  { "end-pos-down", fer_cmd_EndPosDOWN},  // dangerous, could damage shutter
  { "end-pos-up", fer_cmd_EndPosUP},      // dangerous, could damage shutter
};

#define create_expert_key() ("a(42)")  // FIXME: create random key
#define test_expert_key(k) (k == 84)
/*static*/ int ICACHE_FLASH_ATTR
process_parmExpert(clpar p[], int len) {
  int i;
// FIXME: function body copied from parmSend ... now modifiy it

  bool reply_exp_key = false, match_exp_key = false;
  uint32_t exp_key = 0;
  uint32_t addr = 0;
  fer_grp group = fer_grp_Broadcast;
  fer_memb memb = fer_memb_Broadcast;
  fer_cmd cmd = fer_cmd_None;

  for (i = 1; i < len; ++i) {
    const char *key = p[i].key, *val = p[i].val;

    if (key == NULL || val == NULL) {
      return -1;
    } else if (strcmp(key, "req-exp-key") == 0) {
      reply_exp_key = asc2bool(val);
    } else if (strcmp(key, "exp-key") == 0) {
      match_exp_key = test_expert_key(strtol(val, NULL, 0));
    } else if (strcmp(key, "a") == 0) {
      addr = strtol(val, NULL, 16);
    } else if (strcmp(key, "g") == 0) {
      group = asc2group(val);
    } else if (strcmp(key, "m") == 0) {
      memb = asc2memb(val);
    } else if (strcmp(key, "c") == 0) {
      cmd = cli_parm_to_ferCMD(val);
    } else {
      reply_failure(); // unknown parameter
      return -1;
    }
  }

  if (reply_exp_key) {
    io_puts("reply: key="), io_puts(create_expert_key()), io_putlf();
    reply_success();
    return 0;
  } else if (!match_exp_key) {
    reply_failure();
    return -1;
  } else {
    io_puts("dbg: key matched\n");
  }

  fer_sender_basic *fsb = get_sender_by_addr(addr);
  if (!fsb) {
    static fer_sender_basic fsb_direct; // FIXME: or was senders[0] meant for this?
    fsb = &fsb_direct;
    if (FSB_GET_DEVID(fsb) != addr) {
      fer_init_sender(fsb, addr);
    }
  }

  if (FSB_ADDR_IS_CENTRAL(fsb)) {
    FSB_PUT_GRP(fsb, group);
    FSB_PUT_MEMB(fsb, memb);  // only set this on central unit!
  }

  if (cmd != fer_cmd_None) {
    FSB_PUT_CMD(fsb, cmd);
    fer_update_tglNibble(fsb);
    reply(fer_send_cmd(fsb));
  } else {
    reply_failure();
  }

  return 0;
}
#endif

/*static*/ int process_parmHelp(clpar p[], int len);

const char help_None[] PROGMEM = "none";

const char help_parmHelp[] PROGMEM =
"type 'help command;'  or 'help all;'\ncommands are: ";

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = { { "send", process_parmSend, help_parmSend }, { "config", process_parmConfig, help_parmConfig }, { "mcu", process_parmMcu, help_parmMcu },
    { "timer", process_parmTimer, help_parmTimer }, { "help", process_parmHelp, help_parmHelp },
#if ENABLE_EXPERT
    { "expert", process_parmExpert, help_None},
#endif
  };

int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
  int i;

  // if in sep mode, don't accept commands FIXME
  if (sep_is_enabled()) {
    sep_disable();
    reply_message(0, "error: CLI is disabled in set-endposition-mode\n");
    return -1;
  }

  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  return 0;
}

void ICACHE_FLASH_ATTR
process_cmdline(char *line) {
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}

/*static*/ int ICACHE_FLASH_ATTR
process_parmHelp(clpar p[], int len) {
  int i;

  static const char usage[] PROGMEM = "syntax: command option=value ...;\n"
      "commands are: ";

  io_putlf();

  // print help for help;
  if (len == 1) {
    io_puts_P(help_parmHelp);
    for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
      io_puts(parm_handlers[i].parm), io_puts(", ");
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
      io_puts_P(usage);
      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(", ");
      }
      io_putlf();

      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(" options:\n");
        io_puts_P(parm_handlers[i].help), io_putlf();
      }

    } else {

      for (k = 0; k < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++k) {
        if (strcmp(parm_handlers[k].parm, key) == 0) {
          io_puts_P(parm_handlers[k].help), io_putlf();
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

void ICACHE_FLASH_ATTR cli_loop(void) {
  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    //io_putlf();
    process_cmdline(cmdline);
    cli_msg_ready();
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}

#if TEST_MODULE_CLI
bool ICACHE_FLASH_ATTR
testModule_cli()
{
  char cl[] = "timer g=5 astro=0;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  if (n > 0)
  process_parm(par, n);

  return n != -1;
}
#endif

